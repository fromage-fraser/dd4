/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *  Mitchell Tse.                                                          *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/


/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

/*
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern  int     malloc_debug    args((int));
extern  int     malloc_verify   args((void));
#endif

#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
*/
/* this i added back in - Brutus */
#include <dirent.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
/* end that */

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/resource.h>

#include <unistd.h>
#include <stdarg.h>

#include "merc.h"

/*
	Socket and TCP/IP stuff.
*/

#include <signal.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define IAC 255
#define EOR 239

void		pipe_handler(int signal);
void		trap_handler(int signal);
void		scan_object_for_dup(CHAR_DATA *ch, OBJ_DATA *obj);
char	*	prompt_return(CHAR_DATA *, char *);
int		pager(DESCRIPTOR_DATA *, const char *, int, char *);
void		scroll(DESCRIPTOR_DATA *, const char *, int);
char		ansi_strip_txt[MAX_STRING_LENGTH];
bool		scroll_you(DESCRIPTOR_DATA *, const char *, bool);


/*
	OS-dependent declarations.
*/

#if defined(linux)
int	close		args((int fd));
#ifndef __GLIBC__2__
int	getpeername	args((int s, struct sockaddr *name, int *namelen));
int	getsockname	args((int s, struct sockaddr *name, int *namelen));
int	listen		args((int s, int backlog));
#endif

/* int	gettimeofday	args((struct timeval *tp, struct timezone *tzp)); */
int	select		args((int width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout));
int	socket		args((int domain, int type, int protocol));
#endif




/*
 * identd
 */
#if !defined(STDOUT_FILENO)
#define STDOUT_FILENO 1
#endif


const   char    echo_off_str    [] = { IAC, WILL, TELOPT_ECHO, '\0' };
const   char    echo_on_str     [] = { IAC, WONT, TELOPT_ECHO, '\0' };
const   char    go_ahead_str    [] = { IAC, GA, '\0' };

/* GCMP */
#define MAX_STACK_CALL		200
#define MAX_STACK_CALL_LEN	200
char				* str_empty;

char	call_stack[MAX_STACK_CALL][MAX_STACK_CALL_LEN];
int	call_index;
/* end GCMP */

#define     WNOHANG         1       /* Don't block waiting.  */



/*
 * OS-dependent declarations.
 */
#if     defined(interactive)
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

/*int   accept          args((int s, struct sockaddr *addr, int *addrlen)); */
/*int   bind            args((int s, struct sockaddr *name, int namelen)); */
int     close           args((int fd));
/*int   getpeername     args((int s, struct sockaddr *name, int *namelen)); */
/*int   getsockname     args((int s, struct sockaddr *name, int *namelen));*/
/* int  gettimeofday    args((struct timeval *tp, struct timezone *tzp));  Commented out due to compile error/duplication -- Owl 10/2/22*/
/* int  listen          args((int s, int backlog));*/
/* int  read            args((int fd, char *buf, int nbyte));*/
int     select          args((int width, fd_set *readfds, fd_set *writefds,
                            fd_set *exceptfds, struct timeval *timeout));
int     socket          args((int domain, int type, int protocol));
/* int  write           args((int fd, char *buf, int nbyte));*/

void  nonblock     args((int s));

pid_t   waitpid         args((pid_t pid, int *status, int options));
pid_t   fork            args((void));
int     kill            args((pid_t pid, int sig));
int     pipe            args((int filedes[2]));
int     dup2            args((int oldfd, int newfd));
int     execl           args((const char *path, const char *arg, ...));


/*
 * Global variables.
 */

DESCRIPTOR_DATA *   descriptor_free;    /* Free list for descriptors    */
DESCRIPTOR_DATA *   descriptor_list;    /* All open descriptors         */
DESCRIPTOR_DATA *   d_next;             /* Next descriptor in loop      */
FILE *              fpReserve;          /* Reserved file handle         */
bool                merc_down;          /* Shutdown                     */
bool                wizlock;            /* Game is wizlocked            */
int                 numlock = 0;        /* Game is numlocked at <level> */
char                str_boot_time [ MAX_INPUT_LENGTH ];
time_t              current_time;       /* Time of this pulse           */
time_t              boot_time;
int                 mudport;            /* mudport */


void	game_loop_unix			args((void)); /* new GCMP */
int	init_socket			args(());
void	new_descriptor			args((void));

/* void    game_loop_unix          args((int control, int wizPort)); 
int     init_socket             args((int port)); 
void    new_descriptor          args((int control)); */
void    wizPort_handler args((int control));
bool    read_from_descriptor    args((DESCRIPTOR_DATA *d));
/* bool    write_to_descriptor     args((int desc, char *txt, int length)); - changed GCMP */


int connection_count = 0;
DESCRIPTOR_DATA *initiative_list [MAX_CONNECTIONS];


/*
 * Other local functions (OS-independent).
 */
bool		check_reconnecting	args((DESCRIPTOR_DATA *d,CHAR_DATA *ch));
bool    check_parse_name        args((char *name, bool mobcheck));
bool    check_playing           args((DESCRIPTOR_DATA *d, char *name));
int     main                    args((int argc, char **argv));
/* void    nanny                   args((DESCRIPTOR_DATA *d, char *argument)); */
bool		nanny			args((DESCRIPTOR_DATA *d, char *argument));
bool    process_output          args((DESCRIPTOR_DATA *d, bool fPrompt));
void    read_from_buffer        args((DESCRIPTOR_DATA *d));
/*
void    stop_idling             args((CHAR_DATA *ch));
*/
void		stop_idling		args((DESCRIPTOR_DATA *d));
void    bust_a_prompt           args((DESCRIPTOR_DATA *d));
void    assert_directory_exists args((const char *path));


int main(int argc, char **argv)
{
        struct timeval now_time;
        int cntr;
       /* int port; 
        int control;
        int wizPort = 0; */
        push_call("main(%p,%p)",argc,argv);
        /*
         * Memory debugging if needed.
         */
#if defined(MALLOC_DEBUG)
        malloc_debug(2);
#endif

        ALLOCMEM(mud,				MUD_DATA,			1);
        ALLOCMEM(mud->usage,			USAGE_DATA,		1);
	ALLOCMEM(mud->time_info,		TIME_INFO_DATA,	1);
	ALLOCMEM(mud->tactical,		TACTICAL_MAP,		1);
	/* ALLOCMEM(mud->last_player_cmd,	unsigned char, 	MAX_INPUT_LENGTH) */

	init_mth();
        str_empty				= STRALLOC("");

        /*
         * Init time.
         */
        gettimeofday(&now_time, NULL);
        current_time = (time_t) now_time.tv_sec;
        boot_time = current_time;
        strcpy(str_boot_time, ctime(&boot_time));
        mud->current_time = now_time.tv_sec;
	mud->boot_time	   = now_time.tv_sec;
	mud->port         = 8888;

        /*
         * Reserve one channel for our use.
         */
        if (!(fpReserve = fopen(NULL_FILE, "r")))
        {
                perror(NULL_FILE);
                exit(1);
        }

        /*
         * Get the port number.
         */
        
        if (mud->port == 8888)
	{
		SET_BIT(mud->flags, MUD_EMUD_REALGAME);
	}

        if (argc > 1)
        {
                if (!is_number(argv[1]))
                {
                        fprintf(stderr, "Usage: %s [port #]\n", argv[0]);
                        exit(1);
                }
                else if ((mud->port = atoi(argv[1])) <= 1024)
                {
                        fprintf(stderr, "Port number must be above 1024.\n");
                        exit(1);
                }
        }


        /*
         * Check certain directories exist. This helps to avoid surprising errors once the server is running.
         */
        assert_directory_exists(PLAYER_DIR);

        /*
         * Run the game.
         */
      /*  mudport = port; */
     /*   control = init_socket(mud->port); */
        boot_db();
        sprintf(log_buf, "DD4 is ready to rock on port %d.", mud->port);
        log_string(log_buf);
       /* game_loop_unix(control, wizPort);
        close(control);
        */

	if (!IS_SET(mud->flags, MUD_EMUD_COPYOVER))
	{
		sprintf(log_buf, "Initializing socket for port %d", mud->port);
		mud->control = init_socket();
	}

	sprintf(log_buf,"Starting game_loop_unix: control = %d, port = %d", mud->control, mud->port);

	game_loop_unix();

	close(mud->control);

        /*
         * That's all, folks.
         */
        log_string("Normal termination of game.");
        last_command[0] = '\0';
        exit(0);
        return 0;
}

/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
int init_socket(void)
{
	static struct sockaddr_in sa_zero;
	struct sockaddr_in sa;
	int x=1;
	int fd;

	push_call("init_socket(void)");

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("init_socket: socket");
		abort();
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof(x)) < 0)
	{
		perror("init_socket: SO_REUSEADDR");
		close(fd);
		abort();
	}

	{
		struct linger ld;

		ld.l_onoff  = 0;
		ld.l_linger = 100;

		if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld)) < 0)
		{
			perror("init_socket: SO_LINGER");
			close(fd);
			abort();
		}
	}

/*
	{
		int sockbuf;
		unsigned int socksize;

		socksize = sizeof(sockbuf);

		if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *) &sockbuf, &socksize) < 0)
		{
			perror("getsockopt: SO_RCVBUF");
			close(fd);
			abort();
		}

		log_printf("Initial SO_RCVBUF size:  %d", sockbuf);


		if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *) &sockbuf, &socksize) < 0)
		{
			perror("getsockopt: SO_SNDBUF");
			close(fd);
			abort();
		}

		log_printf("Initial SO_SNDBUF size: %d", sockbuf);

		sockbuf = MAX_INPUT_LENGTH;

		if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *) &sockbuf, sizeof(socksize)) < 0)
		{
			perror("getsockopt: SO_RCVBUF");
			close(fd);
			abort();
		}

		if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *) &sockbuf, &socksize) < 0)
		{
			perror("getsockopt: SO_RCVBUF");
			close(fd);
			abort();
		}

		log_printf("Modified SO_RCVBUF size: %d", sockbuf);


		sockbuf = MAX_STRING_LENGTH;

		if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *) &sockbuf, sizeof(socksize)) < 0)
		{
			perror("getsockopt: SO_SNDBUF");
			close(fd);
			abort();
		}

		if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *) &sockbuf, &socksize) < 0)
		{
			perror("getsockopt: SO_SNDBUF");
			close(fd);
			abort();
		}

		log_printf("Modified SO_SNDBUF size: %d", sockbuf);
	}
*/
	sa			= sa_zero;
	sa.sin_family	= AF_INET;
	sa.sin_port	= htons(mud->port);

	if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0)
	{
		perror("init_socket: bind");
		close(fd);
		abort();
	}

	if (listen(fd, 20) < 0)
	{
		perror("init_socket: listen");
		close(fd);
		abort();
	}
	pop_call();
	return fd;
}



/*
 * Here comes the ident driver code.
 * - Wreck
 */
#if defined(unix)

/*
 * Almost the same as read_from_buffer...
 */
bool read_from_ident(int fd, char *buffer)
{
        static char inbuf[MAX_STRING_LENGTH*2];
        int iStart, i, j, k;

        /* Check for overflow. */
        iStart = strlen(inbuf);
        if (iStart >= sizeof(inbuf) - 10)
        {
                log_string("Ident input overflow!!!");
                return FALSE;
        }

        /* Snarf input. */
        for (;;)
        {
                int nRead;

                nRead = read(fd, inbuf + iStart, sizeof(inbuf) - 10 - iStart);
                if (nRead > 0)
                {
                        iStart += nRead;
                        if (inbuf[iStart-2] == '\n' || inbuf[iStart-2] == '\r')
                                break;
                }
                else if (nRead == 0)
                {
                        return FALSE;
                }
                else if (errno == EWOULDBLOCK)
                        break;
                else
                {
                        perror("Read_from_ident");
                        return FALSE;
                }
        }

        inbuf[iStart] = '\0';

        /*
         * Look for at least one new line.
         */
        for (i = 0; inbuf[i] != '\n' && inbuf[i] != '\r'; i++)
        {
                if (inbuf[i] == '\0')
                        return FALSE;
        }

        /*
         * Canonical input processing.
         */
        for (i = 0, k = 0; inbuf[i] != '\n' && inbuf[i] != '\r'; i++)
        {
                if (inbuf[i] == '\b' && k > 0)
                        --k;
                else if (isascii(inbuf[i]) && isprint(inbuf[i]))
                        buffer[k++] = inbuf[i];
        }

        /*
         * Finish off the line.
         */
        if (k == 0)
                buffer[k++] = ' ';
        buffer[k] = '\0';

        /*
         * Shift the input buffer.
         */
        while (inbuf[i] == '\n' || inbuf[i] == '\r')
                i++;
        for (j = 0; (inbuf[j] = inbuf[i+j]) != '\0'; j++)
                ;

    return TRUE;
}

/*
 * Process input that we got from the ident process.
 */
void process_ident(DESCRIPTOR_DATA *d)
{
        char buffer[MAX_INPUT_LENGTH];
        char address[MAX_INPUT_LENGTH];
        CHAR_DATA *ch=CH(d);
        char *user;
        sh_int results=0;
        int status;

        buffer[0]='\0';

        if (!read_from_ident(d->ifd, buffer) || IS_NULLSTR(buffer))
                return;

        /* using first arg since we want to keep case */
        user=first_arg(buffer, address, FALSE);

        /* replace and set some states */
        if (!IS_NULLSTR(user))
        {
                replace_string(d->ident, user);
                SET_BIT(results, 2);
        }
        if (!IS_NULLSTR(address))
        {
                replace_string(d->host, address);
                SET_BIT(results, 1);
        }

        /* do sensible output */
        if (results==1) /* address only */
        {
                /*
                 * Change the two lines below to your notification function...
                 * (wiznet, ..., whatever)
                 *
                 sprintf(outbuf, "$n has address #B%s#b. Username unknown.", address);
                 give_info(outbuf, ch, NULL, NULL, NOTIFY_IDENT, IMMORTAL);
                 */
                sprintf(log_buf, "%s has address %s.", ch->name, address);
                log_string(log_buf);
        }
        else if (results==2 || results==3) /* ident only, or both */
        {
                /*
                 * Change the two lines below to your notification function...
                 * (wiznet, ..., whatever)
                 *
                 sprintf(outbuf, "$n is #B%s@%s#b.", user, address);
                 give_info(outbuf, ch, NULL, NULL, NOTIFY_IDENT, IMMORTAL);
                 */
                sprintf(log_buf, "%s is %s@%s.", ch->name, user, address);
                log_string(log_buf);
        }
        else
        {
                sprintf(log_buf, "%s could not be identified.", ch->name);
                log_string(log_buf);
        }

        /* close descriptor and kill ident process */
        close(d->ifd);
        d->ifd=-1;
        /*
         * we don't have to check here,
         * cos the child is probably dead already. (but out of safety we do)
         *
         * (later) I found this not to be true. The call to waitpid() is
         * necessary, because otherwise the child processes become zombie
         * and keep lingering around... The waitpid() removes them.
         */
        waitpid(d->ipid, &status, WNOHANG);
        d->ipid=-1;

        return;
}


void create_ident(DESCRIPTOR_DATA *d, long ip, sh_int port)
{
        int fds[2];
        pid_t pid;

        /* create pipe first */
        if (pipe(fds) !=0)
        {
                perror("Create_ident: pipe: ");
                return;
        }

        if ((pid = fork()) > 0)
        {
                /* parent process */
                d->ifd=fds[0];
                close(fds[1]);
                d->ipid=pid;
        }

        else if (pid == 0)
        {
                /* child process */
                char str_ip[64], str_local[64], str_remote[64];

                if (dup2 (fds[1], STDOUT_FILENO) != STDOUT_FILENO)
                {
                        perror("Create_ident: dup2(stdout): ");
                        return;
                }

                sprintf(str_local, "%d", mudport);
                sprintf(str_remote, "%d", port);
                sprintf(str_ip, "%ld", ip);

                execl("../src/resolve", "resolve",
                      str_local, str_ip, str_remote, (char *)0);
                /* Still here --> hmm. An error. */
                log_string("Exec failed; Closing child.");
                exit(0);
        }
        else
        {
                /* error */
                perror("Create_ident: fork");
                close(fds[0]);
                close(fds[1]);
        }
}

/*
 *  identd bollocks currently unused
 */

#endif


/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
void game_loop_unix(void)
{
	static struct timeval null_time;
	struct timeval last_time;
	DESCRIPTOR_DATA *d;
	/* char dbuf[180]; */

	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	int maxdesc;

	struct timeval now_time, stall_time;

	int usec_gameloop, usec_gamewait;

	push_call("game_loop_unix(void)");

	gettimeofday(&last_time, NULL);
	mud->current_time = (time_t) last_time.tv_sec;

	stall_time.tv_sec  = 0;

	/*
		Main loop
	*/

	while (!IS_SET(mud->flags, MUD_EMUD_DOWN))
	{
		gettimeofday(&last_time, NULL);

		mud->current_time = last_time.tv_sec;

		if (mud->f_desc == NULL && IS_SET(mud->flags, MUD_EMUD_REBOOT))
		{
			SET_BIT(mud->flags, MUD_EMUD_DOWN);
		}

		alarm(30);

	/*	mud->time = *localtime(&mud->current_time); */

		/*
			Take care of various update routines
		*/

		update_handler();

		start_timer(TIMER_SCAN_DESC);

		/*
			Poll all active descriptors.
		*/

		FD_ZERO(&in_set );
		FD_ZERO(&out_set);
		FD_ZERO(&exc_set);
		FD_SET(mud->control, &in_set);

		maxdesc = mud->control;

		for (d = mud->f_desc ; d ; d = d->next)
		{
			maxdesc = UMAX(maxdesc, d->descriptor);
			FD_SET(d->descriptor, &in_set );
			FD_SET(d->descriptor, &out_set);
			FD_SET(d->descriptor, &exc_set);
		}

		if (select(maxdesc+1, &in_set, &out_set, &exc_set, &null_time) < 0)
		{
			perror("Game_loop: select: poll");
			abort();
		}

		/*
			New connection?
		*/

		if (FD_ISSET(mud->control, &in_set))
		{
			new_descriptor();
		}

		/*
			Kick out the freaky folks.
		*/

		for (d = mud->f_desc ; d ; d = mud->update_gld)
		{
			mud->update_gld = d->next;

			if (FD_ISSET(d->descriptor, &exc_set) || IS_SET(d->mth->comm_flags, COMM_FLAG_DISCONNECT))
			{
				FD_CLR(d->descriptor, &in_set );
				FD_CLR(d->descriptor, &out_set);
				d->outtop = 0;

				if (d->character != NULL)
				{
					log_printf("Kicking off %s for being freaky", d->character->name);
				}
				close_socket(d);
			}
		}

		if (mud->f_desc && !IS_SET(mud->flags, MUD_EMUD_BOOTING))
		{
			for (d = mud->f_desc ; d ; d = d->next)
			{
				REMOVE_BIT(d->mth->comm_flags, COMM_FLAG_COMMAND);
			}
		}
/*
		if (mud->mp_group_greeter && mud->mp_group_greeted)
		{
			if (MP_VALID_MOB(mud->mp_group_greeter))
			{
				mudprog_percent_check(mud->mp_group_greeter->pIndexData->first_prog, mud->mp_group_greeter, mud->mp_group_greeted, NULL, NULL, GROUP_GREET_PROG, mud->mp_group_greeter->pIndexData->vnum);
			}
			mud->mp_group_greeter = mud->mp_group_greeted = NULL;
		}
 removed GCMP*/
		close_timer(TIMER_SCAN_DESC);

		start_timer(TIMER_PROCESS_INPUT);

		/*
			Process input.
		*/

		if (mud->f_desc != NULL && !IS_SET(mud->flags, MUD_EMUD_BOOTING))
		{
			for (d = mud->f_desc ; d ; d = mud->update_gld)
			{
				mud->update_gld = d->next;

				if (FD_ISSET(d->descriptor, &in_set))
				{
					if (d->character == NULL || d->character->desc != d || !IS_SET(CH(d)->pcdata->interp, INTERP_ALIAS))
					{
				                sprintf(log_buf, "DO i get here - process input 1 D%d@%s ", d->descriptor, d->incomm);
                                                log_string(log_buf);
                                                if (!read_from_descriptor(d))
						{
							FD_CLR(d->descriptor, &out_set);
							d->outtop = 0;
							SET_BIT(d->mth->comm_flags, COMM_FLAG_DISCONNECT);
							close_socket(d);
							continue;
						}
					}
				}

				if (d->connected > CON_PLAYING && d->connecting_time + DESCRIPTOR_TIMEOUT < mud->current_time)
				{
					write_to_descriptor(d, "\n\r\n\rLogin time out reached.\n\r\n\r", 0);
					/* log_god_printf("Login timeout reached for D%d@%s", d->descriptor, d->host); */
                                        sprintf(log_buf, "Login timeout reached for D%d@%s", d->descriptor, d->host);
                                        log_string(log_buf);
					close_socket(d);
					continue;
				}
                        
				process_naws(d, d->mth->cols, d->mth->rows); 

				if (d->character != NULL && d->character->wait > 0)
				{
					
                                            sprintf(log_buf, "DO i get here not null and waiting");
                                                log_string(log_buf);
                                                bool HighSpeed = FALSE;

					if (IS_NPC(d->character))
					{
						d->character->wait--;
						if (d->character->wait == 0)
						{
							vt100prompt(d->character);
						}
						continue;
					}

					if (d->incomm[0] == '\0')
					{
						if (d->connected <= CON_PLAYING && d->character && !IS_NPC(d->character) && who_fighting(d->character))
						{
							if (d->character->pcdata->auto_flags == AUTO_QUICK && d->character->hit > d->character->pcdata->wimpy)
							{
								HighSpeed = TRUE;
							}
						}
					}
					if (HighSpeed)
					{
						d->character->wait -= 2;
						if (d->character->wait < 0)
						{
							d->character->wait = 0;
						}
					}
					else
					{
						--d->character->wait;
						if (d->character->wait < 0)
						{
							d->character->wait = 0;
						}
					}
					if (d->character->wait == 0)
					{
						vt100prompt(d->character);
					}
					continue;
				}
                                
				/*
					Reset characters from aliasses - removing - GCMP
				*/

				if (*d->incomm == '\0' && d->connected <= CON_PLAYING && d->character && d == d->character->desc && *d->inbuf == '\0' && d->intop == 0)
				{
					REMOVE_BIT(CH(d)->pcdata->interp, INTERP_ALIAS);

					if (d->back_buf != NULL)
					{
						d->intop = str_cpy_max(d->inbuf, d->back_buf, MAX_INPUT_LENGTH-10);
						STRFREE(d->back_buf);
						d->back_buf = NULL;
					}
					else
					{
						SET_BIT(d->mth->comm_flags, COMM_FLAG_NOINPUT);
					}
				} 
				read_from_buffer(d);
                                

				/*
					Auto reactions
				*/

			/*	if (d->incomm[0] == '\0'
				&&  d->connected == CON_PLAYING
				&&  d->character != NULL
				&&  d->original == NULL
				&&  d == d->character->desc
				&&  d->character->wait == 0
				&&  d->character->in_room != room_index[ROOM_VNUM_LIMBO])
				{
					if (who_fighting(d->character) && d->character->pcdata->auto_flags != AUTO_OFF && d->character->hit > d->character->pcdata->wimpy)
					{
						int sn;
						bool hadtostand	= FALSE;
						bool ms			= FALSE;

						switch(d->character->position)
						{
							case POS_SLEEPING:
							case POS_RESTING:
							case POS_SITTING:
								do_stand(d->character,"");
								hadtostand = TRUE;
								break;
							case POS_STANDING:
								d->character->position = POS_FIGHTING;
								break;
							case POS_FIGHTING:
								break;
							default:
								hadtostand = TRUE;
								break;
						}
						if (hadtostand)
						{
							break;
						}

						if (!str_prefix("mass ", d->character->pcdata->auto_command))
						{
							ms = TRUE;
							sn = skill_lookup(&d->character->pcdata->auto_command[5]);
						}
						else
						{
							sn = skill_lookup(d->character->pcdata->auto_command);
						}

						if (sn != -1)
						{
							if (is_spell(sn))
							{
								if (d->character->mana > 2 * get_mana(d->character, sn))
								{
									SET_BIT(d->character->pcdata->interp, INTERP_AUTO);
									d->character->wait = 2;
									if (ms)
									{
										sprintf(dbuf, "mass '%s'", skill_table[sn].name);
									}
									else
									{
										sprintf(dbuf, "cast '%s'", skill_table[sn].name);
									}
									write_to_buffer(d, "", 0);
									str_cpy_max(d->incomm, dbuf, MAX_INPUT_LENGTH);
								}
							}
							else
							{
								if (d->character->move > 2 * skill_table[sn].cost)
								{
									SET_BIT(d->character->pcdata->interp, INTERP_AUTO);
									d->character->wait = 2;
									sprintf(dbuf, "%s", skill_table[sn].name);
									write_to_buffer(d, "", 0);
									str_cpy_max(d->incomm, dbuf, MAX_INPUT_LENGTH);
								}
							}
						}
						else
						{
							ch_printf(d->character, "%s is not a spell or skill.\n\r", d->character->pcdata->auto_command);
							log_printf("%s has auto command set to: %s", d->character->name, d->character->pcdata->auto_command);
							d->character->pcdata->auto_flags = AUTO_OFF;
							STRFREE(d->character->pcdata->auto_command);
							d->character->pcdata->auto_command = STRALLOC("");
						}
					}
					else if (*d->character->pcdata->tracking != '\0')
					{
						CHAR_DATA * rch;

							Tracking Skill
						

						for (rch = d->character->in_room->first_person ; rch ; rch = rch->next_in_room)
						{
							if (!strcasecmp(rch->name, d->character->pcdata->tracking) || !strcasecmp(rch->short_descr, d->character->pcdata->tracking))
							{
								send_to_char("You've found your prey!\n\r", d->character);

								if (IS_AFFECTED(d->character, AFF_HUNT) && number_percent() < d->character->pcdata->learned[gsn_greater_hunt])
								{
									if (!IS_NPC(rch))
									{
										do_murder(d->character, d->character->pcdata->tracking);
									}
									else
									{
										do_kill(d->character, rch->name);
									}
								}
								STRFREE(d->character->pcdata->tracking);
								d->character->pcdata->tracking = STRALLOC("");
								break;
							}
						}
						if (rch == NULL)
						{
							do_track(d->character, d->character->pcdata->tracking);
						}
					} */
			/*		else if (d->character->pcdata->travel >= 0)
					{
						int exit_cnt, new_exit, cnt;
						CHAR_DATA *ch;

						
							Travel Command
						

					 	if (d->character->position != POS_STANDING || d->character->fighting || d->character->hit < d->character->pcdata->wimpy)
					 	{
							d->character->pcdata->travel		= -1;
							d->character->pcdata->travel_from	= NULL;
							send_to_char("You stop traveling.\n\r", d->character);
						}
						else
						{
							ch		= d->character;
							exit_cnt	= 0;
							new_exit	= 0;
							cnt		= 0;

							for (; cnt < 6 ; cnt++)
							{
								if (cnt != rev_dir[d->character->pcdata->travel] && get_exit(ch->in_room->vnum, cnt) != NULL)
								{
									if (can_move_char(ch, cnt))
									{
										if (ch->pcdata->travel_from == NULL || ch->pcdata->travel_from != room_index[ch->in_room->exit[cnt]->to_room])
										{
											new_exit = cnt;
											exit_cnt++;
										}
									}
								}
							}

							if (exit_cnt != 1)
							{
								if (exit_cnt < 1)
								{
									if (ch->move > 15)
									{
										send_to_char("You stop traveling due to a dead end.\n\r", ch);
									}
									else
									{
										send_to_char("You stop traveling to rest.\n\r", ch);
									}
								}
								else
								{
									send_to_char("You stop traveling to pick directions.\n\r", ch);
								}
								ch->pcdata->travel		= -1;
								ch->pcdata->travel_from	= NULL;
							}
							else
							{
								ch->pcdata->travel		= new_exit;
								ch->pcdata->travel_from	= ch->in_room;

								ch_printf(ch, "You travel %s.\n\r", dir_name[new_exit]);
								 move_char(ch, new_exit, TRUE); 
                                                                move_char(ch, new_exit);
							}
						}
					} */
				/*} */ 

				/*
					Check input
				*/
				if (d->incomm[0] != '\0')
				{
			                sprintf(log_buf, "DO i get here - d to imcomming read_from_buffer 6%12s . D%d@%s %d ",CH(d) ? CH(d)->name : "Nanny" , d->descriptor, d->incomm, d->connected);
                                        log_string(log_buf);
                                        if (IS_SET(d->mth->comm_flags, COMM_FLAG_NOINPUT))
					{
						REMOVE_BIT(d->mth->comm_flags, COMM_FLAG_NOINPUT);
						SET_BIT(d->mth->comm_flags, COMM_FLAG_COMMAND);
					}
					stop_idling(d);

					if (!IS_SET(d->mth->comm_flags, COMM_FLAG_DISCONNECT))
					{
						sprintf(log_buf, "DO i get here - d to imcomming comm_flag_disconnectread_from_buffer 6%12s . D%d@%s %d ",CH(d) ? CH(d)->name : "Nanny" , d->descriptor, d->incomm, d->connected);
                                                log_string(log_buf);
                                                if (d->connected == CON_PLAYING)
						{
							interpret(d->character, d->incomm);
						}
					/*	else if (d->connected == CON_EDITING)
						{
							edit_buffer(d->character, d->incomm);
						} */
						else
						{
							if (nanny(d, d->incomm))
							{
								continue;
							}
						}
					}

				}
			}
		}

		close_timer(TIMER_PROCESS_INPUT);

		/*
			Prompt and Tactical.
		*/

		start_timer(TIMER_TACTICAL_UPD);

		if (mud->f_desc != NULL && !IS_SET(mud->flags, MUD_EMUD_BOOTING))
		{
			for (d = mud->f_desc ; d ; d = mud->update_gld)
			{
				mud->update_gld = d->next;

				*d->incomm = '\0';

				if ((IS_SET(d->mth->comm_flags, COMM_FLAG_COMMAND) || d->outtop > 0) && FD_ISSET(d->descriptor, &out_set))
				{
					if (!process_output(d, TRUE) || IS_SET(d->mth->comm_flags, COMM_FLAG_DISCONNECT))
					{
						d->outtop = 0;
						close_socket(d);
					}
				}
			}
		}


		if (mud->f_desc != NULL && !IS_SET(mud->flags, MUD_EMUD_BOOTING))
		{
			for (d = mud->f_desc ; d ; d = mud->update_gld)
			{
				mud->update_gld = d->next;

				if (!IS_SET(d->mth->comm_flags, COMM_FLAG_DISCONNECT) && d->character && d->connected <= CON_PLAYING && CH(d)->pcdata && IS_SET(CH(d)->pcdata->interp, INTERP_TACT_UPDATE))
				{
					vt100prompter(d->character);
				}
			}
		}

		close_timer(TIMER_TACTICAL_UPD);

		start_timer(TIMER_PROCESS_OUTPUT);

		if (mud->f_desc != NULL && !IS_SET(mud->flags, MUD_EMUD_BOOTING))
		{
			for (d = mud->f_desc ; d ; d = mud->update_gld)
			{
				mud->update_gld = d->next;

				if (IS_SET(d->mth->comm_flags, COMM_FLAG_DISCONNECT))
				{
					continue;
				}
				if (d->outtop)
				{
					if (d->outbuf[d->outtop - 1] != '\r')
					{
						d->outbuf[d->outtop++] = IAC;
						d->outbuf[d->outtop++] = EOR;
					}
					write_to_port(d);
				}
			}
		}

		close_timer(TIMER_PROCESS_OUTPUT);

		/*
			pause the loop
		*/

		gettimeofday(&now_time, NULL);

		if (now_time.tv_sec == last_time.tv_sec)
		{
			usec_gameloop = now_time.tv_usec - last_time.tv_usec;
		}
		else
		{
			usec_gameloop = 1000000 - last_time.tv_usec + now_time.tv_usec;
		}

		usec_gamewait = 1000000 / PULSE_PER_SECOND - usec_gameloop;

		stall_time.tv_usec = usec_gamewait;

		mud->total_io_ticks++;
		mud->total_io_exec  += usec_gameloop;
		mud->total_io_delay += usec_gamewait;

		if (usec_gamewait >= 0)
		{
			if (select(0, NULL, NULL, NULL, &stall_time) < 0)
			{
				perror("Game_loop: select: stall");
			}
		}
		else
		{
			/*
			log_printf("game_loop_unix: heartbeat violation of %lld usec", usec_gameloop - 1000000LL / PULSE_PER_SECOND);
			*/
		}
	}
	pop_call();
	return;
}

/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
void init_descriptor (DESCRIPTOR_DATA *dnew, int desc)
{
	dnew->descriptor			= desc;
	dnew->connected			= CON_GET_NAME;
	dnew->connecting_time		= mud->current_time;
	dnew->port_size			= 10000;
	dnew->inlast				= STRDUPE(str_empty);
}

/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
void new_descriptor(void)
{
	DESCRIPTOR_DATA *dnew, *dtmp;
	BAN_DATA *pban;
	struct sockaddr_in sock;
	int desc, sockbuf;
	socklen_t size;
	struct linger ld;

	size = sizeof(sock);

	push_call("new_descriptor(void)");

	getsockname(mud->control, (struct sockaddr *) &sock, &size);

	if ((desc = accept(mud->control, (struct sockaddr *) &sock, &size)) < 0)
	{
		perror("New_descriptor: accept");
		pop_call();
		return;
	}

	if (fcntl(desc, F_SETFL, O_NDELAY|O_NONBLOCK) == -1)
	{
		perror("new_descriptor: fcntl: O_NDELAY|O_NONBLOCK");
		pop_call();
		return;
	}

	sockbuf = 2048;

	if (setsockopt(desc, SOL_SOCKET, SO_RCVBUF, (char *) &sockbuf, sizeof(sockbuf)) < 0)
	{
		perror("new_socket: SO_RCVBUF");
	}

	ld.l_onoff  = 0;
	ld.l_linger = 100;

	if (setsockopt(desc, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld)) < 0)
	{
		perror("new_socket: SO_LINGER");
	}

	/*
		Cons a new descriptor.
	*/

	ALLOCMEM(dnew, DESCRIPTOR_DATA, 1);

	init_descriptor (dnew, desc);

	init_mth_socket(dnew);

	size = sizeof(sock);

	if (getpeername(desc, (struct sockaddr *) &sock, &size) < 0)
	{
		perror("new_descriptor: getpeername");
		dnew->host   = STRALLOC("(unknown)");
		SET_BIT(dnew->mth->comm_flags, COMM_FLAG_DISCONNECT);
	}
	else
	{
		int addr, address[4];
		char buf[MAX_INPUT_LENGTH];

		addr = ntohl(sock.sin_addr.s_addr);

		address[0] = (addr >> 24) & 0xFF ;
		address[1] = (addr >> 16) & 0xFF ;
		address[2] = (addr >>  8) & 0xFF ;
		address[3] = (addr      ) & 0xFF ;

		sprintf(buf, "%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
		dnew->host = STRALLOC(buf);

		/* log_god_printf("New connect: %s D%d", dnew->host , dnew->descriptor); */
                sprintf(log_buf, "New connect: %s D%d", dnew->host , dnew->descriptor);
                log_string(log_buf);
	}

	/*
		Init descriptor data.
	*/

	for (dtmp = mud->f_desc ; dtmp ; dtmp = dtmp->next)
	{
		if (strcmp(dnew->host, dtmp->host) < 0)
		{
			INSERT_LEFT(dnew, dtmp, mud->f_desc, next, prev);
			break;
		}
	}

	if (dtmp == NULL)
	{
		LINK(dnew, mud->f_desc, mud->l_desc, next, prev);
	}

	mud->total_desc++;

	for (pban = mud->f_ban ; pban ; pban = pban->next)
	{
		if (!str_prefix(pban->name, dnew->host))
		{
			write_to_descriptor(dnew, "\n\rYour site has been banned from this Mud.\n\r\n\r", 0);
		/*	log_god_printf("Ban prefix '%s' for '%s', denying access.", pban->name, dnew->host); */
                        sprintf(log_buf, "Ban prefix '%s' for '%s', denying access.", pban->name, dnew->host);
                        log_string(log_buf);
			close_socket(dnew);

			pop_call();
			return;
		}
	}

	if (mud->total_desc > MAX_CONNECTIONS)  /* Limit descriptors  */
	{
		char buf[MAX_STRING_LENGTH];

		sprintf(buf,"\n\rSorry, the limit of %d players online has been reached.\n\rPlease try back later.\n\r\n\r", MAX_CONNECTIONS);
		write_to_descriptor(dnew, buf, 0);
		close_socket(dnew);
		pop_call();
		return;
	}

	/*
		Inform the client about emud's telnet support
	*/

	/*
		Send the greeting.
	*/
	{
                extern char * help_greeting;

                if (help_greeting[0] == '.')
                        write_to_buffer(dnew, help_greeting+1, 0);
                else
                        write_to_buffer(dnew, help_greeting  , 0);
	}
	pop_call();
	return;
}

/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
void close_socket(DESCRIPTOR_DATA *dclose)
{
	CHAR_DATA *ch;
	DESCRIPTOR_DATA *dt;

	push_call("close_socket(%p)",dclose);

	for (dt = mud->f_desc ; dt ; dt = dt->next)
	{
		if (dt == dclose)
		{
			break;
		}
	}

	uninit_mth_socket(dclose);

	if (dt == NULL)
	{
		log_string("close_socket: unlinked desc called in close_socket");
		pop_call();
		return;
	}

	if (!IS_SET(dclose->mth->comm_flags, COMM_FLAG_DISCONNECT))
	{
		write_to_port(dclose);
	}

	if (dclose->snoop_by != NULL)
	{
		write_to_buffer(dclose->snoop_by, "Your victim has left the game.\n\r", 0);
		dclose->snoop_by = NULL;
	}

	for (dt = mud->f_desc ; dt ; dt = dt->next)
	{
		if (dt->snoop_by == dclose)
		{
			dt->snoop_by = NULL;
		}
	}

	if (dclose->character)
	{
		if (dclose->character->desc)
		{
			do_return(dclose->character, NULL);
		}
		ch = dclose->character;

		/* log_god_printf("Closing link to %s@%s D%d Connected %d.", 
			ch->name,
			dclose->host,
			dclose->descriptor,
			dclose->connected); */

		if ((dclose->connected == CON_PLAYING)
                    || ((dclose->connected >= CON_NOTE_TO)
                        && (dclose->connected <= CON_NOTE_FINISH)))
		{
			act("$n has lost $s link.", ch, NULL, NULL, TO_ROOM);
			ch->desc = NULL;
		}
		else
		{
			dclose->character = NULL;

			if (get_char_pvnum(ch->pcdata->pvnum) == NULL)
			{
				ch->desc = NULL;
				extract_char(ch, TRUE);
			} 
		}
	}

	dclose->outtop = 0;

	if (dclose == mud->update_gld)
	{
		mud->update_gld = dclose->next;
	}

	if ((dclose->prev == NULL && dclose != mud->f_desc)
	||  (dclose->next == NULL && dclose != mud->l_desc))
	{
		log_printf("UNLINK ERROR unlinking descriptor %d.", dclose->descriptor);
	}
	else
	{
		UNLINK(dclose, mud->f_desc, mud->l_desc, next, prev);
	}

	mud->total_desc--;

	close(dclose->descriptor);

	STRFREE(dclose->host);
	STRFREE(dclose->inlast);

	if (dclose->back_buf)
	{
		STRFREE(dclose->back_buf);
	}
	FREEMEM(dclose);

	pop_call();
	return;
}


/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
bool read_from_descriptor(DESCRIPTOR_DATA *d)
{
	CHAR_DATA *ch;
	char bufin[MAX_INPUT_LENGTH];
	int nRead;

	push_call("read_from_descriptor(%p)",d);

	ch = d->original ? d->original : d->character;

	/*
		Hold horses if pending command already.
	*/

	if (d->incomm[0] != '\0')
	{
		pop_call();
		return TRUE;
	}

	if (ch && d->connected <= CON_PLAYING && ch->trust != ch->level)
	{
		ch->trust = ch->level;
	}

	/*
		Was there anything here to begin with ?
	*/

	if (d->back_buf)
	{
		pop_call();
		return(TRUE);
	}

	/*
		Check for overflow.
	*/

	if (d->intop > MAX_INPUT_LENGTH -100)
	{
		sprintf(log_buf, "%s input overflow!", d->host);
                log_string(log_buf);
                       /* log_god_printf("%s input overflow!", d->host); */
		write_to_descriptor(d, "\n\r*** PUT A LID ON IT!!! ***\n\rYou have just overflowed your buffer.  You may get back on the game.\n\r", 0);

		pop_call();
		return FALSE;
	}

	/*
		Snarf input.
	*/

	while (TRUE)
	{
		nRead = read(d->descriptor, bufin, MAX_INPUT_LENGTH);

		bufin[nRead] = 0;

		if (nRead > 0)
		{
			d->intop += translate_telopts(d, (unsigned char *)bufin, nRead, (unsigned char *) d->inbuf, d->intop);

			pop_call();
			return TRUE;
		}
		else if (nRead == 0)
		{
			pop_call();
			return FALSE;
		}
		else if (errno == EWOULDBLOCK)
		{
			break;
		}
		else
		{
			sprintf(log_buf, "Read_from_descriptor D%d@%s errno %d", d->descriptor, d->host, errno);
                        log_string(log_buf);
                       /* log_god_printf("Read_from_descriptor D%d@%s errno %d", d->descriptor, d->host, errno); */
			pop_call();
			return FALSE;
		}
	}
	pop_call();
	return TRUE;
}

/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
void read_from_buffer(DESCRIPTOR_DATA *d)
{
	int i, k;
	CHAR_DATA *ch,*sh;

	/*
		Hold horses if pending command already.
	*/

	push_call("read_from_buffer(%p)",d);

	if (d->incomm[0] != '\0')
	{

                pop_call();
		return;
	}

	if (d->inbuf[0] == '\0')
	{
                if (d->intop > 0)
		{
                        fprintf(stderr, "%12s: ITE: %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
				CH(d) ? CH(d)->name : "Nanny",
				d->inbuf[0], d->inbuf[1], d->inbuf[2],
				d->inbuf[3], d->inbuf[4], d->inbuf[5],
				d->inbuf[6], d->inbuf[7], d->inbuf[8]);

			memmove(&d->inbuf[0], &d->inbuf[1], MAX_INPUT_LENGTH - 1);

			d->intop--;
		}
		pop_call();
		return;
	}

	ch = d->original ? d->original : d->character;

	/*
		Look for at least one new line.
	*/

	if (d->intop < MAX_INPUT_LENGTH -10)
	{
	sprintf(log_buf, "DO i get here - read_from_buffer5 . %12s . D%d@%s  ",CH(d) ? CH(d)->name : "Nanny" , d->descriptor, d->incomm);
        log_string(log_buf);
        for (i = 0 ; d->inbuf[i] != '\n' ; i++)
		{
			sprintf(log_buf, "DO i get here - read_from_buffer 6%12s . D%d@%s %d ",CH(d) ? CH(d)->name : "Nanny" , d->descriptor, d->incomm, d->connected);
                        log_string(log_buf);
                        if (i >= d->intop)
			{
							sprintf(log_buf, "DO i get here - read_from_buffer 7");
                        log_string(log_buf);
                                pop_call();
				return;
			}

			if (d->inbuf[i] == '\0')
			{
				/* log_god_printf("Read_from_buffer D%d@%s NULL byte in input string.", d->descriptor, d->host); */
                                sprintf(log_buf, "Read_from_buffer D%d@%s NULL byte in input string.", d->descriptor, d->incomm);
                                log_string(log_buf);
				write_to_buffer(d, "Input error.\n\r", 0);
				
				d->inbuf[0] = 0;
				d->intop = 0;

				pop_call();
				return;
			}
		}
	}

	for (i = k = 0 ; d->inbuf[i] != '\n' ; i++)
	{
			sprintf(log_buf, "DO i get here - read_from_buffer 8 . %12s . D%d@%s %d ",CH(d) ? CH(d)->name : "Nanny" , d->descriptor, d->host, d->connected);
                        log_string(log_buf);
                        if (i >= MAX_INPUT_LENGTH - 20)
		{
			write_to_buffer(d, "Line too long.\n\r", 0);

			d->inbuf[i]    = '\n';
			d->inbuf[i+1]  = '\0';
			d->intop       = i+1;
			break;
		}

		if (d->inbuf[i] == '\b' && k > 0)
		{
			--k;
		}
		else
		{
			d->incomm[k++] = d->inbuf[i];
		}
	}

	/*
		Finish off the line.
	*/
	if (k == 0)
	{
		d->incomm[k++] = ' ';
	}

	d->incomm[k] = '\0';

	/*
		Do '!' substitution.
	*/

/*	if (d->connected == CON_PLAYING && !IS_SET(ch->pcdata->interp, INTERP_ALIAS))
	{
		if (d->incomm[0] == '.' || d->incomm[0] == '!')
		{
			if (isalpha(d->incomm[1]))
			{
				str_cpy_max(d->incomm, ch->pcdata->back_buf[tolower(d->incomm[1]) - 'a'], MAX_INPUT_LENGTH);
			}
			else
			{
				strcpy(d->incomm, d->inlast);
			}
		}
		else
		{
			if (isalpha(d->incomm[0]))
			{
				RESTRING(d->inlast, d->incomm);

				STRFREE(ch->pcdata->back_buf[tolower(d->incomm[0]) - 'a']);
				ch->pcdata->back_buf[tolower(d->incomm[0]) - 'a'] = STRDUPE(d->inlast);
			}
		}
	}
*/
	if (d->snoop_by && d->character)
	{
		sh = d->snoop_by->original ? d->snoop_by->original : d->snoop_by->character;

		if (sh && sh->desc && sh->desc->character == sh)
		{
			ch_printf_color(sh, "{168}%s {078}[{178}%s{078}]\n\r", ch->name, d->incomm);
		}
	}

	d->intop -= i + 1;

	memmove(d->inbuf, d->inbuf + i + 1, d->intop);

	d->inbuf[d->intop] = 0;


	pop_call();
	return;
}


void ch_printf_color(CHAR_DATA *ch, const char *fmt, ...)
{
	char buf[MAX_STRING_LENGTH];
	va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	send_to_char_color(buf, ch);
}

void send_to_char_color(char *txt, CHAR_DATA *ch)
{
	push_call("send_to_char_color(%p,%p)",txt,ch);

	send_to_char(ansi_translate_text(ch, txt), ch);

	pop_call();
	return;
}

/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
bool process_output(DESCRIPTOR_DATA *d, bool fPrompt)
{
	char buf[MAX_STRING_LENGTH];

	push_call("process_output(%p,%p)",d,fPrompt);

	if (d == NULL)
	{
		pop_call();
		return FALSE;
	}

	/*
		Bust a prompt.
	*/

	if (fPrompt && !IS_SET(mud->flags, MUD_EMUD_DOWN) && d->connected == CON_PLAYING)
	{
		CHAR_DATA *ch, *sh;

		ch = CH(d);
		sh = d->character;

		if (!is_desc_valid(sh))
		{
	  		pop_call();
			return FALSE;
		}

		if (!HAS_BIT(ch->pcdata->vt100_flags, VT100_INTERFACE))
		{
			if (IS_SET(ch->act, PLR_BLANK))
			{
				sprintf(buf, "%s\n\r", get_color_string(ch, COLOR_PROMPT, VT102_DIM));
			}
			else
			{
				strcpy(buf, "");
			}

			strcat(buf, prompt_return(sh, ch->pcdata->prompt_layout));

			SET_BIT(ch->pcdata->interp, INTERP_SCROLL);

			write_to_buffer(d, buf, 1000000);

			REMOVE_BIT(ch->pcdata->interp, INTERP_SCROLL);
		}
		else
		{
			vt100prompt(ch);
		}
	}
	pop_call();
	return TRUE;
}

/*
	The buffer that works with the page pauser
*/
/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
int pager(DESCRIPTOR_DATA *d, const char *istr, int lng, char *ostr)
{
	int lines, pt, breakpt, lengt, cnt;
	char pag_buf[MAX_STRING_LENGTH], buf[MAX_INPUT_LENGTH], lcc[10];
	register char *ptt, *pto, *pti;

	push_call("pager(%p,%p,%p,%p)",d,istr,lng,ostr);

	if (d->connected >= CON_PLAYING
	||  CH(d)->pcdata->last_command
	||  IS_SET(CH(d)->act, PLR_PAGER)
	||  IS_SET(CH(d)->pcdata->interp, INTERP_PAGE)
	||  IS_SET(CH(d)->pcdata->interp, INTERP_SCROLL))
	{
		REMOVE_BIT(CH(d)->pcdata->interp, INTERP_PAUSE);

		pop_call();
		return (str_cpy_max(ostr, istr, MAX_STRING_LENGTH));
	}

	for (ptt = (char *)istr, pt = 0, lines = 0 ; pt < lng ; pt++, ptt++)
	{
		if (*ptt == '\n')
		{
			lines++;
		}
	}

	breakpt = get_pager_breakpt(CH(d));

	if (lines <= breakpt+1)
	{
		REMOVE_BIT(CH(d)->pcdata->interp, INTERP_PAUSE);

		pop_call();
		return (str_cpy_max(ostr, istr, MAX_STRING_LENGTH));
	}

	SET_BIT(CH(d)->pcdata->interp, INTERP_PAUSE);

	pti = (char *)istr;
	pto = ostr;

	lcc[0] = '\0';

	for (pt = 0, lines = 0 ; lines < breakpt ; pt++, pti++, pto++)
	{
		*pto = *pti;

		if (*pti == '\033' && *(pti+1) == '[')
		{
			for (cnt = 0 ; *(pti+cnt) != 'm' && *(pti+cnt) != '\0' ; cnt++)
			{
				lcc[cnt] = *(pti+cnt);
			}
			lcc[cnt] = 'm';
			cnt++;
			lcc[cnt] = '\0';
		}
		if (*pti == '\n')
		{
			lines++;
		}
	}

	if (*pti == '\r')
	{
		*pto = *pti;
		pto++;
		pti++;
		pt++;
	}

	lengt = pt;

	*pto = '\0';

	for (pto = pag_buf, cnt = 0 ; lcc[cnt] != '\0' ; cnt++, pto++)
	{
		*pto = lcc[cnt];
	}

	for (; pt < lng ; pto++, pti++, pt++)
	{
		*pto = *pti;
	}
	*pto = '\0';

	if (!IS_SET(CH(d)->pcdata->vt100_flags, VT100_INTERFACE)) 
	{
		/* sprintf(buf, "%s --------------------------[Press Return to Continue]--------------------------%s\n\r", ansi_translate_text(CH(d), "{128}"), ansi_translate_text(CH(d), "{300}")); */
		sprintf(buf, "--------------------------[Press Return to Continue]--------------------------\n\r"); 
                str_apd_max(ostr, buf, lengt, MAX_STRING_LENGTH);
	}
	STRFREE(CH(d)->pcdata->page_buf);
	CH(d)->pcdata->page_buf = STRALLOC(pag_buf);

	SET_BIT(CH(d)->pcdata->interp, INTERP_PAGE);

	pop_call();
	return(lengt);
}

/*
	The buffer used for the grep command.
*/
/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
void scroll(DESCRIPTOR_DATA *d, const char *txi, int lng)
{
	PC_DATA *pch;
	char buf[MAX_STRING_LENGTH], *pti, *pto;

	pti = (char *) txi;
	pto = (char *) buf;

	push_call("scroll(%p,%p,%p)",d,txi,lng);

	if (d->connected != CON_PLAYING)
	{
		pop_call();
		return;
	}

	pch = CH(d)->pcdata;

	if (IS_SET(pch->interp, INTERP_SCROLL))
	{
		pop_call();
		return;
	}

	while (*pti)
	{
		if (pti[0] == '\n' && pti[1] == '\r')
		{
			if (pti[2] == '\033')
			{
				*pto = 0;

				pch->scroll_buf[pch->scroll_end] = STRALLOC(buf);

				pch->scroll_end++;

				if (pch->scroll_end >= MAX_SCROLL_BUF)
				{
					pch->scroll_beg = 1;
					pch->scroll_end = 0;
				}

				if (pch->scroll_buf[pch->scroll_end])
				{
					STRFREE(pch->scroll_buf[pch->scroll_end]);
				}

				pto = buf;
				pti += 2;
			}
			else if (pti[2] == 0)
			{
				*pto = 0;

				pch->scroll_buf[pch->scroll_end] = STRALLOC(buf);

				pch->scroll_end++;

				if (pch->scroll_end >= MAX_SCROLL_BUF)
				{
					pch->scroll_beg = 1;
					pch->scroll_end = 0;
				}

				if (pch->scroll_buf[pch->scroll_end])
				{
					STRFREE(pch->scroll_buf[pch->scroll_end]);
				}
				break;
			}
			else
			{
				*pto++ = *pti++;
			}
		}
		else
		{
			*pto++ = *pti++;
		}
	}


	pop_call();
	return;
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt(DESCRIPTOR_DATA *d)
{
        CHAR_DATA *ch;
        const char *str;
        const char *i;
        char      *point;
        char      *pbuff;
        char       buffer[ MAX_STRING_LENGTH ];
        char       buf  [ MAX_STRING_LENGTH ];
        char       buf2 [ MAX_STRING_LENGTH ];

        if (d->original)
        {
                sprintf (buf, "[Switched as %s] ", d->character->short_descr);
                write_to_buffer (d, buf, 0);
                return;
        }

        ch = d->character;
        if (!ch->prompt || ch->prompt[0] == '\0')
        {
                send_to_char_bw("\n\r\n\r", ch);
                return;
        }

        point = buf;
        str = ch->prompt;

        while(*str != '\0')
        {
                if(*str != '%')
                {
                        *point++ = *str++;
                        continue;
                }
                ++str;

                switch(*str)
                {
                    default:
                        i = " ";
                        break;

                    case '1':
                        i = "";
                        if(IS_SET(ch->act, PLR_ANSI)) i = BLUE;
                        break;
                    case '2':
                        i = "";
                        if(IS_SET(ch->act, PLR_ANSI)) i = GREEN;
                        break;
                    case '3':
                        i = "";
                        if(IS_SET(ch->act, PLR_ANSI)) i = CYAN;
                        break;
                    case '4':
                        i = "";
                        if(IS_SET(ch->act, PLR_ANSI)) i = RED;
                        break;
                    case '5':
                        i = "";
                        if(IS_SET(ch->act, PLR_ANSI)) i = PURPLE;
                        break;
                    case '6':
                        i = "";
                        if(IS_SET(ch->act, PLR_ANSI)) i = YELLOW;
                        break;
                    case '7':
                        i = "";
                        if(IS_SET(ch->act, PLR_ANSI)) i = GREY;
                        break;
                    case 'B':
                        i = "";
                        if(IS_SET(ch->act, PLR_ANSI) || IS_SET(ch->act, PLR_VT100)) i = BOLD;
                        break;
                    case 'N':
                        i = "";
                        if(IS_SET(ch->act, PLR_ANSI) || IS_SET(ch->act, PLR_VT100)) i = NTEXT;
                        break;

                    case 'h' :
                        sprintf(buf2, "%d", ch->hit);
                        i = buf2;
                        break;

                    case 'H' :
                        sprintf(buf2, "%d", ch->max_hit);
                        i = buf2;
                        break;

                    case 'm' :
                        sprintf(buf2, "%d", ch->mana);
                        i = buf2;
                        break;

                    case 'M' :
                        sprintf(buf2, "%d", ch->max_mana);
                        i = buf2;
                        break;

                    case 'v' :
                        sprintf(buf2, "%d", ch->move);
                        i = buf2;
                        break;

                    case 'V' :
                        sprintf(buf2, "%d", ch->max_move);
                        i = buf2;
                        break;

                    case 'p' :
                        sprintf(buf2, "%s", position_name(ch->position));
                        i = buf2;
                        break;

                    case 'q' :
                        sprintf(buf2, "%d", ch->rage);
                        i = buf2;
                        break;

                    case 'Q' :
                        sprintf(buf2, "%d", ch->max_rage);
                        i = buf2;
                        break;

                    case 'e' :
                        if (ch->level >= LEVEL_HERO)
                                sprintf (buf2, "(n/a)");
                        else
                                sprintf(buf2, "%d", level_table[ch->level].exp_total - ch->exp);
                        i = buf2;
                        break;

                    case 'x' :
                        sprintf(buf2, "%d", ch->exp);
                        i = buf2;
                        break;

                    case 'g' :
                        sprintf(buf2, "%d", total_coins_char(ch) );
                        i = buf2;
                        break;

                    case 'w' :
                        sprintf(buf2, "%d", ch->wait);
                        i = buf2;
                        break;

                    case 'a' :
                        sprintf(buf2, "%s", IS_GOOD(ch) ? "good"
                                : IS_EVIL(ch) ? "evil" : "neutral");
                        i = buf2;
                        break;

                    case 'A' :
                        if (ch->level > 9)
                                sprintf(buf2, "%d", ch->alignment);
                        else
                                sprintf (buf2, "?");
                        i = buf2;
                        break;

                    case 'f' :
                        if (ch->class == CLASS_SHAPE_SHIFTER)
                        {
                                if (ch->sub_class == SUB_CLASS_VAMPIRE)
                                        sprintf (buf2, "%s", is_affected (ch, gsn_mist_walk)
                                                 ? "mist" : "normal");
                                else
                                        sprintf (buf2, "%s", extra_form_name (ch->form));
                        }
                        else
                                sprintf (buf2, " ");
                        i = buf2;
                        break;

                    case 't' :
                        if (ch->pcdata->countdown)
                                sprintf (buf2, "%d", ch->pcdata->countdown);
                        else
                                sprintf (buf2, "no quest");
                        i = buf2;
                        break;

                    case 'T' :
                        if (ch->pcdata->nextquest > 0 || !ch->pcdata->countdown)
                                sprintf (buf2, "%d", ch->pcdata->nextquest);
                        else
                                sprintf (buf2, "questing");
                        i = buf2;
                        break;

                    case 'r' :
                        if(ch->in_room)
                                sprintf(buf2, "%s", ch->in_room->name);
                        else
                                sprintf(buf2, " ");
                        i = buf2;
                        break;

                    case 'R' :
                        if(ch->level > LEVEL_HERO && ch->in_room)
                                sprintf(buf2, "%d", ch->in_room->vnum);
                        else
                                sprintf(buf2, " ");
                        i = buf2;
                        break;

                    case 'z' :
                        if(ch->in_room)
                                sprintf(buf2, "%s", ch->in_room->area->name);
                        else
                                sprintf(buf2, " ");
                        i = buf2;
                        break;

                    case 'i' :
                        sprintf(buf2, "%s", IS_AFFECTED(ch, AFF_INVISIBLE) ?
                                "invisible" : "visible");
                        i = buf2;
                        break;

                    case 'W' :
                        if(ch->level > LEVEL_HERO)
                                sprintf(buf2, "(wizinv: %s)",
                                        IS_SET(ch->act, PLR_WIZINVIS) ? "on" : "off");
                        else
                                sprintf(buf2, " ");
                        i = buf2;
                        break;

                    case 's':
                        if (!IS_NPC(ch))
                        {
                                if (IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
                                        sprintf(buf2, "{Yarena{x");
                                else if ((IS_SET(ch->status, PLR_RONIN) || ch->clan)
                                         && ch->level > 14
                                         && !(IS_SET(ch->in_room->room_flags, ROOM_SAFE)
                                              || IS_SET(ch->in_room->area->area_flags, AREA_FLAG_SAFE)))
                                        sprintf(buf2, "{Runsafe{x");
                                else
                                        sprintf(buf2, "{Gsafe{x");
                        }
                        else
                                sprintf(buf2, " ");
                        i = buf2;
                        break;

                    case 'S':
                        if (!IS_NPC(ch))
                        {
                                if (IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
                                        sprintf(buf2, "{Yarena{x");
                                else if (IS_SET(ch->in_room->room_flags, ROOM_SAFE)
                                         || IS_SET(ch->in_room->area->area_flags, AREA_FLAG_SAFE))
                                        sprintf(buf2, "{Gsafe{x");
                                else
                                        sprintf(buf2, "{Runsafe{x");
                        }
                        else
                                sprintf(buf2, " ");
                        i = buf2;
                        break;
                    case 'G':
                        if (!IS_NPC(ch))
                        {
                                if (ch->pcdata->meter >= 0)
                                {
                                        int last;
                                        int first;
                                        last = (ch->pcdata->meter %10 );
                                        first = (ch->pcdata->meter);
                                        while(first >= 10)
                                        {
                                                first = first / 10;
                                        }
                                        
                                        if (ch->pcdata->meter == 0)
                                                sprintf(buf2, "[  <51>0%%<0> ]");
                                        else if (ch->pcdata->meter < 10)
                                                sprintf(buf2, "[<154><454>-<0> <51>%d%%<0> ]", last);
                                        else if (ch->pcdata->meter < 20)
                                                sprintf(buf2, "[<154><454>-<0><51>%d%d%%<0> ]", first, last);
                                        else if (ch->pcdata->meter < 40)
                                                sprintf(buf2, "[<154><454>-<0><51><448>%d<0><51>%d%%<0> ]", first, last);
                                        else if (ch->pcdata->meter < 60)
                                                sprintf(buf2, "[<154><454>-<0><51><448>%d<0><51><442>%d<0><51>%%<0> ]", first, last);
                                        else if (ch->pcdata->meter < 80)
                                                sprintf(buf2, "[<154><454>-<0><51><448>%d<0><51><442>%d<436>%%<0> ]", first, last);
                                        else if (ch->pcdata->meter < 100)
                                                sprintf(buf2, "[<154><454>-<0><51><448>%d<0><51><442>%d<436>%%<0><124><424>-<0>]", first, last);
                                        else if (ch->pcdata->meter >= 100)
                                                sprintf(buf2, "[<556><309><15>100%%<0><309><9>-<0>]");
                                }
                        }
                        else
                                sprintf(buf2, " ");
                        i = buf2;
                        break;
                    case 'k' :
                        if (ch->pcdata->dam_meter >= 0)
                                sprintf(buf2, "%d/%d%%", ch->pcdata->dam_meter, ch->damage_enhancement );
                        else
                                sprintf(buf2, " ");
                        i = buf2;
                        break;
                    case 'b':
                        if (!IS_NPC(ch) && ch->pcdata->blink)
                                sprintf(buf2, "on");
                        else
                                sprintf(buf2, "off");
                        i = buf2;
                        break;

                    case '%' :
                        sprintf(buf2, "%%");
                        i = buf2;
                        break;
                }
                ++str;
                while((*point = *i) != '\0')
                        ++point, ++i;
        }

        /*write_to_buffer(d, buf, point - buf); */

        *point = '\0';
        pbuff = buffer;
        colourconv_8bit(pbuff, buf, ch);

        write_to_buffer(d, buffer, 0);
        return;

}

/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
void write_to_buffer(DESCRIPTOR_DATA *d, char *txt, int length)
{
	char buf[MAX_STRING_LENGTH];
	char txo[MAX_STRING_LENGTH];
	CHAR_DATA *ch, *sh;
	int size;

	push_call("write_to_buffer(%p,%p,%p)",d,txt,length);

	if (d == NULL || txt == NULL)
	{
		pop_call();
		return;
	}

	ch = CH(d);

	if (IS_SET(mud->flags, MUD_SKIPOUTPUT) && length != 1000000)
	{
		if (ch == NULL || IS_NPC(ch) || ch->level < MAX_LEVEL )
		{
			pop_call();
			return;
		}
	}

	if (ch)
	{
		if ((IS_SET(ch->act, PLR_ANSI) || IS_SET(ch->act, PLR_VT100)) && d->connected >= CON_PLAYING && length != 1000000)
		{
			if (txt[0] == '\0')
			{
				pop_call();
				return;
			}
			size = UMIN(strlen(txt), MAX_STRING_LENGTH - 200);

			size = pager(d, txt, size, txo);

			if (length != 1000000)
			{
				scroll(d, txo, size);
			}

			buf[0] = 0;

			if (d->outtop == 0 && IS_SET(d->mth->comm_flags, COMM_FLAG_COMMAND))
			{
				strcat(buf, "\033[0K");
			}

			if (d->outtop == 0)
			{
				cat_sprintf(buf, "\0337\033[%d;1H%s%s\0338", ch->pcdata->screensize_v - 2, IS_SET(ch->act, PLR_BLANK) ? "\n\r" : "", txo);
                                printf(log_buf, "\0337\033[%d;1H%s%s\0338", ch->pcdata->screensize_v - 2, IS_SET(ch->act, PLR_BLANK) ? "\n\r" : "", txo);
                                log_string(log_buf);
			}
			else
			{
				cat_sprintf(buf, "\0337\033[%d;1H%s\0338", ch->pcdata->screensize_v - 2, txo);
                                printf(log_buf, "\0337\033[%d;1H%s\0338", ch->pcdata->screensize_v - 2, txo);
                                log_string(log_buf);
			}

			if (IS_SET(ch->pcdata->interp, INTERP_PAUSE))
			{
				/* cat_sprintf(buf, "%s --------------------------[Press Return to Continue]--------------------------%s", ansi_translate_text(ch, "{128}"), get_color_string(ch, COLOR_PROMPT, VT102_DIM)); */
                                cat_sprintf(buf, "---------------1----------[Press Return to Continue]--------------------------" ); 
			}
		}
		else
		{
			if ((IS_SET(ch->act, PLR_ANSI) || IS_SET(ch->act, PLR_VT100)) && d->connected >= CON_PLAYING && d->outtop == 0 && !IS_SET(d->mth->comm_flags, COMM_FLAG_COMMAND))
			{
				d->outbuf[0] = '\n';
				d->outbuf[1] = '\r';
				d->outbuf[2] = '\0';
				d->outtop = 2;
			}

			size = UMIN(strlen(txt), MAX_STRING_LENGTH - 100);

			size = pager(d, txt, size, txo);

			if (length != 1000000)
			{
				scroll(d, txo, size);
			}
			strcpy(buf, txo);
		}

		if (d->snoop_by && length != 1000000)
		{
			sh = CH(d->snoop_by);

			if (sh && sh->desc && sh->desc->character == sh)
			{
				if (IS_SET(CH(d)->pcdata->interp, INTERP_PAUSE))
				{
					send_to_char(txo, sh);
				}
				else
				{
					send_to_char(txt, sh);
				}
			}
		}
	}
	else
	{
		if (d->outtop == 0 && d->connected >= CON_PLAYING && !IS_SET(d->mth->comm_flags, COMM_FLAG_COMMAND))
		{
			d->outbuf[0] = '\n';
			d->outbuf[1] = '\r';
			d->outbuf[2] = '\0';
			d->outtop    = 2;
		}
		strcpy(buf, txt);
	}

	d->outtop = str_apd_max(d->outbuf, buf, d->outtop, MAX_STRING_LENGTH);

	pop_call();
	return;
}

/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
bool write_to_descriptor(DESCRIPTOR_DATA *d, char *txt, int length)
{
	push_call("write_to_descriptor(%p,%p,%p)",d,txt,length);

	if (length == 0)
	{
		length = UMIN(strlen(txt), MAX_STRING_LENGTH - d->outtop - 10);
	}
	else
	{
		length = UMIN(length, MAX_STRING_LENGTH - d->outtop - 10);
	}
	
        memcpy(&d->outbuf[d->outtop], txt, length);

	d->outtop += length;

	write_to_port(d);

	pop_call();
	return TRUE;
}


/*
	Write text to the file descriptor utilizing the port size feature.
	Used to flush the outbuf buffer to the actual socket.
	Chaos 5/17/95
*/
/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
void write_to_port(DESCRIPTOR_DATA *d)
{
	int nWrite, tWrite, nBlock, failure = FALSE;

	push_call("write_to_port(%p)",d);

	if (IS_SET(d->mth->comm_flags, COMM_FLAG_DISCONNECT))
	{
		pop_call();
		return;
	}

	if (d->mth->mccp2)
	{
		write_mccp2(d, d->outbuf, d->outtop);

		d->outtop = 0;

		pop_call();
		return;
	}

	for (tWrite = 0 ; tWrite < d->outtop ; tWrite += nWrite)
	{
		nBlock = UMIN(d->outtop - tWrite, d->port_size);

		if ((nWrite = write(d->descriptor, d->outbuf + tWrite, nBlock)) < 1)
		{
			if (errno != EAGAIN && errno != ENOSR)
			{
			/*	log_god_printf("write_to_port D%d@%s", d->descriptor, d->host); */
                                sprintf(log_buf, "write_to_port D%d@%s", d->descriptor, d->host);
                                log_string(log_buf);
				dump_stack();
				SET_BIT(d->mth->comm_flags , COMM_FLAG_DISCONNECT);
			}
			else
			{
				failure = TRUE;
			}
			break;
		}
	}

	if (failure)
	{
		if (++d->timeout < 60)
		{
		/*	log_god_printf("write_to_port: timeout D%d@%s", d->descriptor, d->host); */
                        sprintf(log_buf, "write_to_port: timeout D%d@%s", d->descriptor, d->host);
                        log_string(log_buf);
			SET_BIT(d->mth->comm_flags , COMM_FLAG_DISCONNECT);
		}
		else
		{
			pop_call();
			return;
		}
	}

	d->timeout = 0;

	mud->total_io_bytes += tWrite;

	d->outtop	 = 0;
	pop_call();
	return;
}


/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
bool nanny(DESCRIPTOR_DATA *d, char *argument)
{
	char buf[MAX_STRING_LENGTH],buf2[MAX_STRING_LENGTH];
	BAN_DATA *pban;
	CHAR_DATA *ch;
                char      *p;
                char      *pwdnew;
	int iClass, iRace, cnt, count, fOld;
        int        startcoinsamt;
        COIN_DATA *startcoins;
        char      *classname;
                DESCRIPTOR_DATA *temp;
        startcoins = (COIN_DATA *) malloc (sizeof(COIN_DATA));

	push_call_format("nanny(%d,%s)",d->connected,argument);

	while (isspace(*argument))
	{
		argument++;
	}

	ch = d->character;

	nanny:

	switch (d->connected)
	{
		default:
			bug("Nanny: bad d->connected %d.", d->connected);
			close_socket(d);

			pop_call();
			return FALSE;

		case CON_GET_NAME:
			if (argument[0] == '\0')
			{
				close_socket(d);
				pop_call();
				return FALSE;
			}

			argument[0] = UPPER(argument[0]);

			if (!check_parse_name(argument, FALSE))
			{
				write_to_buffer(d, "\n\rIf you wish to create a new character, enter 'NEW' as your name.\n\r\n\rWho art thou: ", 1000000);

				pop_call();
				return FALSE;
			}

			ch = lookup_char(argument);

			d->character = NULL;
			fOld         = FALSE;

			remove_bad_desc_name(argument); /* kick out connections with this name */

			if (ch == NULL || ch->pcdata == NULL)
			{
				fOld = load_char_obj(d, argument);

				if (d->character == NULL)
				{
					write_to_buffer(d, "Your character is faulty.  Contact the Gods.\n\r", 0);
					close_socket(d);

					pop_call();
					return FALSE;
				}
				ch        = d->character;
				d->lookup = FALSE;

				if (IS_SET(mud->flags, MUD_WIZLOCK) && !IS_IMMORTAL(ch))
				{
					write_to_buffer(d, "The game is locked, try later.\n\r", 0);
					close_socket(d);

					pop_call();
					return FALSE;
				}

				if (ch->name == NULL || strcasecmp(ch->name, argument))
				{
					write_to_buffer(d, "Your character is faulty.  Contact the Gods.\n\r", 0);
					close_socket(d);

					pop_call();
					return FALSE;
				}
			}
			else
			{
				fOld         = TRUE;
				d->character = ch;
				d->lookup    = TRUE;
			}

			if (ch->name == NULL || ch->pcdata == NULL)
			{
				log_string("Found nullified character");
				close_socket(d);

				pop_call();
				return FALSE;
			}

			if (!strcasecmp(argument, "new"))
			{
				/* for (pban = mud->f_nban ; pban ; pban = pban->next)
				{
					if (!str_prefix(pban->name, d->host))
					{
						write_to_descriptor(d, "\n\rYou may not create new characters from this site.\n\r\n\rWho art thou: ", 0);

						pop_call();
						return FALSE;
					}
				} */

				free_char(d->character);
				d->character = NULL;

				display_empty_screen(d);

				write_to_buffer(d, "Suitable names are an essential part of maintaining a role-playing environment.\n\r", 1000000);
				write_to_buffer(d, "Make sure to pick a name that suits a medieval fantasy theme, and that is not:\n\r\n\r", 1000000);
				write_to_buffer(d, " - Nonsensical, unpronouncable, ridiculous, profane, or derogatory.\n\r", 1000000);
				write_to_buffer(d, " - Contemporary or futuristic, for example 'John' or 'Robot'.\n\r", 1000000);
				write_to_buffer(d, " - Comprised of ranks or titles, for example 'Lady' or 'Sir'.\n\r", 1000000);
				write_to_buffer(d, " - Composed of singular descriptive nouns, adverbs or adjectives, as in\n\r   'Heart', 'Big', 'Flying', 'Death', or 'Broken'\n\r", 1000000);
				write_to_buffer(d, " - Any combination of singular descriptive nouns/adverbs or adjectives, for\n\r", 1000000);
				write_to_buffer(d, " - example 'Heartbound', 'SunStone', or 'Truesword'\n\r\n\r", 1000000);

				write_to_buffer(d, "If the name you select is not acceptable or otherwise unsuitable, you will be\n\rasked to pick another one.\n\r\n\r", 1000000);

				write_to_buffer(d, "Please choose a name for your character: ", 1000000);

				d->connected = CON_GET_NEW_NAME;

				pop_call();
				return FALSE;
			}

			if (fOld)
			{
				/*
					Old player
				*/

				/* if (IS_SET(pvnum_index[ch->pcdata->pvnum]->flags, PVNUM_DENIED))
				{
					log_god_printf("Denying access to %s@%s.", argument, d->host); 
                                        sprintf(log_buf, "Denying access to %s@%s.", argument, d->host); 
                                log_string(log_buf);

					write_to_buffer(d, "You are denied access.\n\r", 0);
					close_socket(d);
					pop_call();
					return FALSE;
				} */

				send_echo_off(d);

				write_to_buffer(d, "Password: ", 1000000);
				d->connected = CON_GET_OLD_PASSWORD;

				pop_call();
				return FALSE;
			}
			else
			{
				write_to_buffer(d, "\n\rIf you wish to create a new character, enter 'NEW' as your name.\n\r\n\rWho art thou: ", 1000000);
				free_char(d->character);
				d->character = NULL;
				pop_call();
				return FALSE;
			}
			break;

		case CON_GET_NEW_NAME:
			if (!strcasecmp(argument, "new"))
			{
				write_to_buffer(d, "\n\rIllegal name, try another.\n\r\n\rName: ", 1000000);
				pop_call();
				return FALSE;
			}

			if (!check_parse_name(argument, TRUE))
			{
				write_to_buffer(d, "\n\rYou have chosen an illegal name.  Please try another.\n\r\n\rNew name: ", 1000000);
				pop_call();
				return FALSE;
			}

			remove_bad_desc_name(argument); /* kick out unconnected sessions with same name */

			if (load_char_obj(d, argument))
			{
				if (d->character != NULL)
				{
					free_char(d->character);
				}
				d->character = NULL;

				write_to_buffer(d, "\n\rYou have chosen a name that already exists.  Please try another.\n\r\n\rNew name: ", 1000000);

				pop_call();
				return FALSE;
			}
			else
			{
				send_echo_off(d);

				ch_printf_color(d->character, "\n\r{078}You must now choose a password.  This password must contain at least five\n\rcharacters, with at least one of them being a number.\n\r\n\rGive me a good password for %s: ", d->character->name);

				d->connected = CON_GET_NEW_PASSWORD;
			}
			break;

		case CON_GET_OLD_PASSWORD:
			if (ch->name == NULL || ch->pcdata == NULL)
			{
				log_string("Found nullified character");
				close_socket(d);
				pop_call();
				return FALSE;
			}
			/* if (encrypt(argument) != ch->pcdata->pwd && encrypt64(argument) != ch->pcdata->pwd) */

			{
				ch_printf_color(ch, "\n\r{078}Wrong password.");

				if (d->lookup)
				{
					close_socket(d);
				}
				else
				{
					close_socket(d);
				}
				pop_call();
				return FALSE;
			}
			ch->pcdata->pwd = encrypt64(argument);

			/*
				Determine if it's a new guy over limit
			*/

			if (mud->total_plr >= MAX_LINKPERPORT && !d->lookup && !IS_IMMORTAL(ch))
			{
				ch_printf_color(ch, "\n\r\n\r{078}The game currently has the maximum amount of %d players online\n\rTry back in a few minutes.\n\r", MAX_LINKPERPORT);
				close_socket(d);
				pop_call();
				return FALSE;
			}
			send_echo_on(d);

			write_to_buffer(d, "\n\r", 0);

			if (check_reconnecting(d, ch))
			{
				
                /*                log_god_printf("%s@%s has reconnected.  D%d", ch->name, d->host, d->descriptor); */
                                sprintf(log_buf, "%s@%s has reconnected.  D%d", ch->name, d->host, d->descriptor); 
                                log_string(log_buf);
				ch->desc = d;

				d->connected       = CON_PLAYING;
				d->character       = ch;
				d->incomm[0]       = 0;
				ch->pcdata->interp = 0;

				RESTRING(ch->pcdata->host, d->host);

				d->lookup = FALSE;

				if (HAS_BIT(ch->pcdata->vt100_flags, VT100_INTERFACE))
				{
					do_refresh(ch, "");
				}

				send_to_char("You've been reconnected.\n\r",  ch);

				pop_call();
				return TRUE;
			}
			else
			{
				ch = d->original ? d->original : d->character;
				ch->desc = d;

				if (ch->in_room == NULL)
				{
					ch->in_room = get_room_index(ROOM_VNUM_TEMPLE);
				}
				do_help(ch, "motd");

				d->connected = CON_READ_MOTD;

				d->lookup = FALSE;

				for (cnt = 0, count = 0 ; cnt < MAX_CLASS ; cnt++)
				{
					count += d->character->pcdata->mclass[cnt];
				}

				if (count != d->character->level && d->character->level < 97 && d->character->level > 10)
				{
					ch_printf_color(ch, "\n\r{078}Your player file has some serious flaws.  Please talk to the gods about this\n\rproblem.  You will not be able to use this character until it is fixed.\n\r");
					log_printf("%s has faulty multi-classed character.", d->character->name);

					close_socket(d);
				}
			}
			break;

		case CON_GET_NEW_PASSWORD:
				if (strlen(argument) < 5)
				{
					ch_printf_color(ch, "\n\r\n\r{078}Password must be at least five characters long.\n\r\n\rPassword: ");

					pop_call();
					return FALSE;
				}

				pwdnew = crypt(argument, ch->name);
                                for (p = pwdnew; *p != '\0'; p++)
                                {
                                        if (*p == '~')
                                        {
                                                write_to_buffer(d, "New password isn't acceptable, please try again.\n\rPassword: ", 0);
                                                return;
                                        }
                                }
                 /*               if (!is_valid_password(argument))
				{
					ch_printf_color(ch, "\n\r\n\r{078}That password is not acceptable, try again.\n\r\n\rPasswords may only contain letters (case sensitive), or numbers.\n\rAt least one number is required in the password.\n\r\n\rPassword: ");

					pop_call();
					return FALSE;
				}
                        */
				ch->pcdata->pwd = encrypt64(argument);

				ch_printf_color(ch, "\n\r\n\r{078}Please retype your password to confirm: ");

				d->connected = CON_CONFIRM_NEW_PASSWORD;
				break;

		case CON_CONFIRM_NEW_PASSWORD:
				if (encrypt64(argument) != ch->pcdata->pwd)
				{
					ch_printf_color(ch, "\n\r\n\r{078}The two passwords didn't match.\n\r\n\rPlease retype your password: ");
					d->connected = CON_GET_NEW_PASSWORD;

					pop_call();
					return FALSE;
				}

				send_echo_on(d);

				ch_printf_color(ch, "\n\r\n\r{078}Do you wish to use ANSI color? (Y/N) {118}");

				d->connected = CON_ANSI;
				break;

		case CON_ANSI:
			switch (tolower(*argument))
			{
				case 'y':
					ch->pcdata->vt100_flags = VT100_FAST|VT100_HIGHLIGHT|VT100_BOLD|VT100_BEEP|VT100_UNDERLINE|VT100_FLASH|VT100_REVERSE|VT100_ANSI;
					break;

				case 'n':
					break;

				default:
					ch_printf_color(ch, "\n\r{078}Please type Yes or No? {118}");

					pop_call();
					return FALSE;
			}

			display_empty_screen(d);

			ch_printf_color(ch, "{118}Your client does not support the VT100 protocol.{078}\033[1G\033[0K\n\r\n\r");

			ch_printf_color(ch, "{078}Lowlands boasts a specialized user interface based on the VT100\n\r");
			ch_printf_color(ch, "{078}terminal emulation protocol. It offers status bars at the top and\n\r");
			ch_printf_color(ch, "{078}bottom of the screen to display information about your character and\n\r");
			ch_printf_color(ch, "{078}surroundings. Clients known to support the VT100 protocol are Zmud,\n\r");
			ch_printf_color(ch, "{078}PuTTY, telnet, and TinTin++.\n\r\n\r");
			ch_printf_color(ch, "{078}Do you wish to use the VT100 interface? (Y/N) {118}");

			d->connected = CON_VT100;

			break;

		case CON_VT100:
			switch (tolower(*argument))
			{
				case 'y':
					SET_BIT(ch->pcdata->vt100_flags, VT100_INTERFACE);
					break;

				case 'n':
					break;

				default:
					ch_printf_color(ch, "\n\r{078}Please type Yes or No? {118}");

					pop_call();
					return FALSE;
			}

			reset_color(ch);
			display_empty_screen(d);

			ch_printf_color(ch, "\n\r{038}You may choose from the following genders:\n\r\n\r{138}    M{068} - Male\n\r{138}    F{068} - Female\n\r\n\r{038}Select your sex: {118}");

			d->connected = CON_GET_NEW_SEX;

			break;

		case CON_GET_NEW_SEX:
			switch (argument[0])
			{
				case 'm':
				case 'M':
					ch->sex = SEX_MALE;
					break;
				case 'f':
				case 'F':
					ch->sex = SEX_FEMALE;
					break;
				default:
					ch_printf_color(ch, "\n\r{018}That's not a sex.\n\r\n\r{038}Select your sex: {118}");
					write_to_buffer(d, (char *)ansi_translate_text(ch, buf), 1000000);

					pop_call();
					return FALSE;
			}
			
			display_empty_screen(d);
                	display_class_selections(d); 

			d->connected = CON_GET_NEW_CLASS;
			break;

		case CON_GET_NEW_CLASS:
			argument = one_argument(argument, buf2);

                        if (!strcasecmp(buf2, "help"))
                        {
                                for (iClass = 0 ; iClass < MAX_CLASS ; iClass++)
                                {
                                        if (!str_prefix(argument, class_table[iClass].show_name))
                                        {
                                                display_class_details(d, iClass);

                                                pop_call();
                                                return FALSE;
                                        }
                                }
                                strcpy(buf, "\n\r{018}No help on that topic.\n\r\n\r{038}Please choose a class: {118}");
                                write_to_buffer(d, (char *)ansi_translate_text(ch,buf), 1000000);

                                pop_call();
                                return FALSE;
                        }

                        for (iClass = 0 ; iClass < MAX_CLASS ; iClass++)
                        {
                                if (!str_prefix(buf2, class_table[iClass].show_name))
                                {
                                        break;
                                }
                        }

                        if (iClass == MAX_CLASS)
                        {
                                strcpy(buf, "\n\r{018}That's not a class.\n\r\n\r{038}Please choose a class: {118}");
                                write_to_buffer(d, (char *)ansi_translate_text(ch,buf), 1000000);

                                pop_call();
                                return FALSE;
                        }

                        ch->class = iClass;

                        display_empty_screen(d);
                       /* display_race_selections(d); */
                        do_help(ch, "race");

                        d->connected = CON_GET_NEW_RACE;
                        break;

		case CON_GET_NEW_RACE:

				argument = one_argument(argument, buf2);

				if (!strcasecmp(buf2, "help"))
				{
					for (iRace = 0 ; iRace < MAX_RACE; iRace++)
					{

						if (!str_prefix(argument, race_table[iRace].race_name))
						{
							strcpy(buf, race_table[iRace].race_name);
							strcat(buf, "intro");
							do_help(ch, buf);

						/*	display_race_details(d, iRace); */



							pop_call();
							return FALSE;
						}
					}
					strcpy(buf, "\n\r{018}No help on that topic.\n\r\n\r{038}Please choose a race: {118}");
					write_to_buffer(d, (char *)ansi_translate_text(ch,buf), 1000000);

					pop_call();
					return FALSE;
				}

 				for (iRace = 0 ; iRace < MAX_RACE ; iRace++)
				{
					if (!str_prefix(buf2, race_table[iRace].race_name))
					{
						break;
					}
				}

				if (iRace == MAX_RACE)
				{
					strcpy(buf, "\n\r{018}That's not a race.\n\r\n\r{038}Please choose a race: {118}");
					write_to_buffer(d, (char *)ansi_translate_text(ch,buf), 1000000);

					pop_call();
					return FALSE;
				}


				ch->race = iRace;

/*				d->connected = CON_APPEAR_LOOK;

				display_appearance_selections(ch, APPEAR_LOOK);
*/

				
                        d->connected = CON_CONFIRM_NEW_RACE;
                        break;

                case CON_CONFIRM_NEW_RACE:
                        switch (argument[0])
                        {
                        case 'y': case 'Y': break;
                        default:
                                send_to_char("\n\r\n\r{038}Select a race{118}\n\r\n\r", ch);
                                do_help (ch, "race");
                                send_to_char("\n\r{038}Please choose a race for your character. [a-x]{118} ", ch);
                                d->connected = CON_GET_NEW_RACE;
                                return FALSE;
                        }
                        send_to_char("\n\r\n\r{W}bGenerate random attribute scores{x\n\r\n\r", ch);
                        do_help(ch, "random");
                        send_to_char("\n\r{cPress {CENTER{X{c to begin rolling your character's attributes.{x ", ch);

                        d->connected = CON_GENERATE_STATS;
                        break;

                case CON_GENERATE_STATS:
                        generate_stats (ch);

                        sprintf(buf, "\n\rStr: %2d  Int: %2d  Wis: %2d  Dex: %2d  Con: %2d\n\r"
                                "{cAccept? {C[y/n]{x ",
                                ch->pcdata->perm_str,
                                ch->pcdata->perm_int,
                                ch->pcdata->perm_wis,
                                ch->pcdata->perm_dex,
                                ch->pcdata->perm_con);
                        send_to_char(buf, ch);
                        d->connected = CON_CONFIRM_STATS;
                        break;

                case CON_CONFIRM_STATS:
                        switch (argument[0])
                        {
                        case 'y': case 'Y': break;
                        default:
                                generate_stats (ch);

                                sprintf(buf, "\n\rStr: %2d  Int: %2d  Wis: %2d  Dex: %2d  Con: %2d\n\r"
                                        "{cAccept? {C[y/n]{x ",
                                        ch->pcdata->perm_str,
                                        ch->pcdata->perm_int,
                                        ch->pcdata->perm_wis,
                                        ch->pcdata->perm_dex,
                                        ch->pcdata->perm_con);
                                send_to_char(buf, ch);
                                return FALSE;
                        }

                        send_to_char("\n\r{WCharacter generation complete.{x\n\r\n\r"
                                "You are now ready to enter the Dragons Domain and begin your training!\n\r\n\r", ch);
                        SET_BIT(ch->act, PLR_AUTOLEVEL);
                        ch->pcdata->pagelen = 100;
                        d->connected = CON_READ_MOTD;
                        break;
				

		case CON_READ_MOTD:
			/*if (new_notes(ch))
			{
				write_to_buffer(d, ansi_translate_text(ch, "{506}There are new notes.{088}\n\r\n\r"), 0);
			}*/

			if (ch->pcdata->pvnum == 0)
			{
				ch->pcdata->pvnum = find_free_pvnum();

				add_pvnum(ch);
				/* save_players(); */
			}

			add_char(ch);
			add_player(ch);

		/*	if (get_room_index(ch->pcdata->recall) == NULL)
			{
				ch->pcdata->recall = ROOM_VNUM_TEMPLE;
			}

			if (get_room_index(ch->pcdata->death_room) == NULL)
			{
				ch->pcdata->death_room = ROOM_VNUM_TEMPLE;
			}
                */
                        SET_BIT(ch->act, PLR_AUTOEXIT);
                        ch->next = char_list;
                        char_list = ch;
			d->connected = CON_PLAYING;

			if (ch->level == 0)
			{
				OBJ_DATA *obj;
                                sprintf(log_buf, "New player %s@%s has entered the game.", ch->name, d->host);
                                log_string(log_buf);
                               ch->level = 1;
                                ch->exp = 1000;

                        /* give the player the base knowledge - geoff */
                                ch->pcdata->learned[gsn_mage_base + ch->class] = 30;

                                /* set initial train to prime req */
                                ch->pcdata->stat_train = class_table[ch->class].attr_prime;

                                ch->pcdata->learned[skill_lookup(race_table[ch->race].spell_one)] = 99;
                                ch->pcdata->learned[skill_lookup(race_table[ch->race].spell_two)] = 99;

                                startcoinsamt   = 50 + number_fuzzy(3)
                                        * number_fuzzy(4) * number_fuzzy(5) * 9;

                                coin_crunch (startcoinsamt, startcoins);
                                ch->plat = startcoins->plat;
                                ch->gold = startcoins->gold;
                                ch->silver = startcoins->silver;
                                ch->copper = startcoins->copper;
                                free (startcoins);

                                /**
                                 * Shade 17.6.22
                                 *
                                 * Make it easier for starting characters; will edit here in the level 0 code not in clear_char
                                 *
                                 * Give them a few of each pracs to get going, make starting hits 50 not 20
                                 */


                                ch->max_hit = 50;
                                ch->hit = ch->max_hit;

                                ch->pcdata->str_prac = 2;
                                ch->pcdata->int_prac = 2;

                                if (ch->class == CLASS_CLERIC || ch->class == CLASS_MAGE || ch->class == CLASS_PSIONICIST)
                                {
                                        ch->pcdata->int_prac += 3;
                                        ch->max_mana = 150;
                                }

                                ch->mana        = ch->max_mana;
                                ch->move        = ch->max_move;

                                set_title(ch, " is a newbie");
                                free_string(ch->prompt);
                                ch->prompt = str_dup("<<{G%h/%H{x hits {C%m/%M{x mana {Y%v/%V{x move [{W%z{x]> ");

                                obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0);
                                obj_to_char(obj, ch);
                                equip_char(ch, obj, WEAR_LIGHT);

                                obj = create_object(get_obj_index(OBJ_VNUM_POUCH), 0);
                                obj_to_char(obj, ch);
                                equip_char(ch, obj, WEAR_POUCH);

                                obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0);
                                obj_to_char(obj, ch);
                                equip_char(ch, obj, WEAR_BODY);

                                if (ch->class != CLASS_THIEF
                                && ch->class != CLASS_BRAWLER
                                && ch->class != CLASS_MAGE)
                                {
                                        obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0);
                                        obj_to_char(obj, ch);
                                        equip_char(ch, obj, WEAR_SHIELD);
                                }

                                if (ch->class != CLASS_BRAWLER)
                                {
                                        obj = create_object( get_obj_index(class_table[ch->class].weapon), 0);
                                        obj_to_char(obj, ch);
                                        equip_char(ch, obj, WEAR_WIELD);
                                        SET_BIT(ch->act, PLR_AUTOWIELD);
                                }

                                if (ch->class == CLASS_RANGER)
                                {
                                        obj = create_object (get_obj_index(OBJ_VNUM_SCHOOL_BOW), 0);
                                        obj_to_char(obj, ch);
                                        equip_char(ch, obj, WEAR_RANGED_WEAPON);
                                }

                                char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));

				if (HAS_BIT(ch->pcdata->vt100_flags, VT100_INTERFACE))
				{
					vt100prompt(ch);
				}
				/* ch->speed = get_max_speed(ch); */
			}
                        else if (ch->in_room)
                        {
                                char_to_room(ch, ch->in_room);
                        }
                        else
                        {
                                char_to_room(ch, get_room_index(DEFAULT_RECALL));
                        }

                        if (ch->level == 1)
                        {
                                sprintf(buf,"%s, a %s of the %s race, is born unto the Domain.",
                                        ch->name,
                                        full_class_name(ch->class),
                                        race_table[ch->race].race_name);
                                do_info(ch, buf);
                        }

                        if (!IS_SET(ch->act, PLR_WIZINVIS) && ch->level > 1)
                        {
                                sprintf(buf, "%s has entered the Dragons Domain...", ch->name);
                                do_info(ch,  buf);
                        }

                        /*
                        *  Entry lag and warning; Gez & Shade 2000
                        */
                        if (ch->level <= LEVEL_HERO)
                        {
                                for (d = descriptor_list; d; d = d->next)
                                {
                                        if (d->connected == CON_PLAYING
                                        && d->character
                                        && d->character != ch)
                                        {
                                                if (d->character->in_room == ch->in_room)
                                                {
                                                        sprintf(buf, "\n\r{R%s has appeared in the room!{x\n\r\n\r",
                                                                ch->name);
                                                        send_to_char(buf, d->character);
                                                }
                                                else if (d->character->in_room->area == ch->in_room->area)
                                                {
                                                        sprintf(buf, "\n\r{R%s has appeared in the area!{x\n\r\n\r",
                                                                ch->name);
                                                        send_to_char(buf, d->character);
                                                }
                                        }
                                }
                        }

                        if (IS_SET(ch->act, PLR_QUESTOR))
                        {
                                REMOVE_BIT(ch->act, PLR_QUESTOR);
                                ch->pcdata->questgiver = NULL;
                                ch->pcdata->countdown  = 0;
                                ch->pcdata->questmob   = 0;
                                ch->pcdata->questobj   = 0;
                        }

                        if (ch->level > 1)
                                do_board (ch, "");

                        send_to_char( "\n\r", ch );
                        do_look(ch, "auto");

                        /*  Entry lag  */
                        if (ch->level <= LEVEL_HERO)
                        {
                                send_to_char("\n\r{RYou must take a few moments to adjust to your new surroundings...{x\n\r", ch);
                                WAIT_STATE(ch, PULSE_VIOLENCE);
                        }

                        if (ch->silent_mode)
                                send_to_char("\n\rYou are in silent mode.\n\r", ch);

                        break;

                case CON_NOTE_TO:
                        handle_con_note_to (d, argument);
                        break;

                case CON_NOTE_SUBJECT:
                        handle_con_note_subject (d, argument);
                        break;

                case CON_NOTE_EXPIRE:
                        handle_con_note_expire (d, argument);
                        break;

                case CON_NOTE_TEXT:
                        handle_con_note_text (d, argument);
                        break;

                case CON_NOTE_FINISH:
                        handle_con_note_finish (d, argument);
                        break;

                case CON_GET_DISCONNECTION_PASSWORD:
        #if defined(unix)
                        write_to_buffer(d, "\n\r", 2);
        #endif
                        if (ch->pcdata->pwd != NULL
                        && strlen(ch->pcdata->pwd) > 0
                        && strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
                        {
                                close_socket(d);
                                return;
                        }

                        write_to_buffer(d, echo_on_str, 0);

                        for (temp = descriptor_list; temp; temp = temp->next)
                        {
                                if (temp != d
                                && temp->character
                                && temp->connected != CON_GET_NAME
                                && temp->connected != CON_GET_DISCONNECTION_PASSWORD
                                && !str_cmp(temp->original ? temp->original->name
                                                : temp->character->name, ch->name)
                                && !temp->character->deleted)
                                {
                                        write_to_buffer(temp, "Another player has disconnected you.\n\r", 0);
                                        free_char(d->character);
                                        d->character = temp->character;
                                        d->character->desc = d;
                                        close_socket(temp);
                                }
                        }

                        sprintf(log_buf, "%s@%s has reconnected themselves",
                                d->character->name,
                                d->host);
                        log_string(log_buf);
                        send_to_char("Reconnecting...\n\r", d->character);
                        act("$n has reconnected.", d->character, NULL, NULL, TO_ROOM);
                        d->connected = CON_PLAYING;
                        break;
	}
	pop_call();
	return FALSE;
}



/*
bool nanny (DESCRIPTOR_DATA *d, char *argument)
{
        CHAR_DATA *ch;
        char      *pwdnew;
        char      *classname;
        char      *p;
        char       buf [ MAX_STRING_LENGTH ];
        int        iClass;
        bool       fOld;
        int        startcoinsamt;
        COIN_DATA *startcoins;
        DESCRIPTOR_DATA *temp;
        startcoins = (COIN_DATA *) malloc (sizeof(COIN_DATA));

        push_call_format("nanny(%d,%s)",d->connected,argument);
        sprintf(log_buf, "nanny(%d,%s)",d->connected,argument);
        log_string(log_buf);
        * Delete leading spaces UNLESS character is writing a note *
        if (d->connected != CON_NOTE_TEXT)
        {
                while (isspace(*argument))
                        argument++;
        }

        * This is here so we wont get warnings.  ch = NULL anyways - Kahn *
        ch = d->character;

        nanny: 
        
        switch (d->connected)
        {
            default:
                bug("Nanny: bad d->connected %d.", d->connected);
                close_socket(d);
                return FALSE;

            case CON_GET_NAME:
                if (argument[0] == '\0')
                {
                        close_socket(d);
                        return FALSE;
                }

                argument[0] = UPPER(argument[0]);
                fOld = load_char_obj(d, argument);
                ch = d->character;

                if (!check_parse_name(argument))
                {
                        if (!fOld)
                        {
                                write_to_buffer(d, "Illegal name: try another.\n\rName: ", 0);
                                return FALSE;
                        }
                        else
                        {
                                *
                                 * Trap the "." and ".." logins.
                                 * Chars must be > level 1 here
                                 *
                                if (ch->level < 1)
                                {
                                        write_to_buffer(d, "Illegal name: try another.\n\rName: ", 0);
                                        return FALSE;
                                }
                                sprintf(buf, "Illegal name:  %s", argument);
                                bug(buf, 0);
                        }
                }

                if (IS_SET(ch->act, PLR_DENY))
                {
                        sprintf(log_buf, "Denying access to %s@%s.", argument, d->host);
                        log_string(log_buf);
                        write_to_buffer(d, "You are denied access.\n\r", 0);
                        close_socket(d);
                        return FALSE;
                }

                if (check_reconnect(d, argument, FALSE))
                        fOld = TRUE;

                else
                {
                        if (wizlock && ch->level <= LEVEL_HERO)
                        {
                                write_to_buffer(d, "The game is wizlocked.\n\r", 0);
                                close_socket(d);
                                return FALSE;
                        }

                        if (ch->level <= numlock && numlock != 0)
                        {
                                write_to_buffer(d, "The game is locked to your level character.\n\r\n\r", 0);
                                close_socket(d) ;
                                return FALSE;
                        }
                }

                if (check_playing(d, ch->name))
                {
                        if (d->connected != CON_GET_DISCONNECTION_PASSWORD)
                                close_socket(d);

                        return FALSE;
                }

                if (fOld)
                {
                        * Old player *
                        write_to_buffer(d, "Password: ", 0);
                        * write_to_buffer(d, echo_off_str, 0); *
                        send_echo_off(d);
                        d->connected = CON_GET_OLD_PASSWORD;
                        return FALSE;
                }
                else
                {
                        * New player *
                        sprintf(buf, "Did I get that right, %s? [y/n] ", argument);
                        write_to_buffer(d, buf, 0);
                        d->connected = CON_CONFIRM_NEW_NAME;
                        return  FALSE;
                }
                break;

            case CON_GET_OLD_PASSWORD:
* #if defined(unix)
                write_to_buffer(d, "\n\r", 2);
#endif *
                if (ch->pcdata->pwd != NULL
                    && strlen(ch->pcdata->pwd) > 0
                    && strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
                {
                        write_to_buffer(d, "Wrong password.\n\r", 0);
                        close_socket(d);
                        return FALSE;
                }

                send_echo_on(d);
                write_to_buffer(d, "\n\r", 0);
                * write_to_buffer(d, echo_on_str, 0); *


                if (check_reconnect(d, ch->name, TRUE))
                        return TRUE;

                sprintf(log_buf, "%s@%s has connected (on desc %d).",
                        ch->name, d->host, d->descriptor);
                log_string(log_buf);

                if (IS_HERO(ch))
                        do_help(ch, "imotd");

                do_help(ch, "motd");

                affect_strip(ch, gsn_mount);
                d->connected = CON_READ_MOTD;
                break;

            case CON_CONFIRM_NEW_NAME:
                switch (*argument)
                {
                    case 'y': case 'Y':
                        sprintf(buf, "\n\rNew character.\n\rEnter a password for %s: %s",
                                ch->name, echo_off_str);
                        write_to_buffer(d, buf, 0);
                        d->connected = CON_GET_NEW_PASSWORD;
                        break;

                    case 'n': case 'N':
                        write_to_buffer(d, "Ok, what IS it, then? ", 0);
                        free_char(d->character);
                        d->character = NULL;
                        d->connected = CON_GET_NAME;
                        break;

                    default:
                        write_to_buffer(d, "Please type YES or NO: ", 0);
                        break;
                }
                break;

            case CON_GET_NEW_PASSWORD:
#if defined(unix)
                write_to_buffer(d, "\n\r", 2);
#endif

                if (strlen(argument) < 5)
                {
                        write_to_buffer(d, "Password must be at least five characters long.\n\rPassword: ", 0);
                        return FALSE;
                }

                pwdnew = crypt(argument, ch->name);
                for (p = pwdnew; *p != '\0'; p++)
                {
                        if (*p == '~')
                        {
                                write_to_buffer(d, "New password isn't acceptable, please try again.\n\rPassword: ", 0);
                                return FALSE;
                        }
                }

                free_string(ch->pcdata->pwd);
                ch->pcdata->pwd = str_dup(pwdnew);
                write_to_buffer(d, "Please retype the password: ", 0);
                d->connected = CON_CONFIRM_NEW_PASSWORD;
                break;

            case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
                write_to_buffer(d, "\n\r", 2);
#endif

                if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
                {
                        write_to_buffer(d, "The passwords didn't match.\n\r\n\rPlease enter a password: ", 0);
                        d->connected = CON_GET_NEW_PASSWORD;
                        return FALSE;
                }

                send_echo_on(d);
                * write_to_buffer(d, echo_on_str, 0); *

                write_to_buffer(d, "Do you want to enable colour? [y/n] ",0);
                d->connected = CON_CHECK_ANSI;
                break;

            case CON_CHECK_ANSI:
                send_echo_on(d);
                * write_to_buffer(d, echo_on_str, 0); *


                switch (*argument)
                {
                    case 'y': case 'Y':
                        SET_BIT(ch->act, PLR_ANSI);
                        break;

                    case 'n': case 'N':
                        break;

                    default:
                        write_to_buffer(d, "Do you want to enable colour? [y/n] ",0);
                        return FALSE;
                }

                write_to_buffer(d, "\n\r", 2);
                do_help(ch, "welcomenote");
                send_to_char("\n\r{cPress {C[Enter]{x{c to create your character.{x ", ch);
                d->connected = CON_WELCOME_RACE;
                ch->race = RACE_HUMAN;
                break;

            case CON_WELCOME_RACE:
                send_to_char("\n\r\n\r{W}bSelect a race{x\n\r\n\r", ch);
                do_help(ch, "races");
                send_to_char("\n\r{cPlease choose a race for your character. {C[a-y]{x ", ch);
                d->connected = CON_GET_NEW_RACE;
                break;

            case CON_GET_NEW_RACE:

                if (argument[0] == '\0')
                {
                        send_to_char("\n\r\n\r{W}bSelect a race{x\n\r\n\r", ch);
                        do_help(ch, "race");
                        send_to_char("\n\r{cPlease choose a race for your character. {C[a-y]{x ", ch);
                        break;
                }

                switch (argument[0])
                {
                    case 'a': case 'A': ch->race = 1;       break;
                    case 'b': case 'B': ch->race = 2;       break;
                    case 'c': case 'C': ch->race = 3;       break;
                    case 'd': case 'D': ch->race = 4;       break;
                    case 'e': case 'E': ch->race = 5;       break;
                    case 'f': case 'F': ch->race = 6;       break;
                    case 'g': case 'G': ch->race = 7;       break;
                    case 'h': case 'H': ch->race = 8;       break;
                    case 'i': case 'I': ch->race = 9;       break;
                    case 'j': case 'J': ch->race = 10;      break;
                    case 'k': case 'K': ch->race = 11;      break;
                    case 'l': case 'L': ch->race = 12;      break;
                    case 'm': case 'M': ch->race = 13;      break;
                    case 'n': case 'N': ch->race = 14;      break;
                    case 'o': case 'O': ch->race = 15;      break;
                    case 'p': case 'P': ch->race = 16;      break;
                    case 'q': case 'Q': ch->race = 17;      break;
                    case 'r': case 'R': ch->race = 18;      break;
                    case 's': case 'S': ch->race = 19;      break;
                    case 't': case 'T': ch->race = 20;      break;
                    case 'u': case 'U': ch->race = 21;      break;
                    case 'v': case 'V': ch->race = 22;      break;
                    case 'w': case 'W': ch->race = 23;      break;
                    case 'x': case 'X': ch->race = 24;      break;
                    case 'y': case 'Y': ch->race = 25;      break;

                    default:
                        send_to_char("\n\r{Y}rInvalid race!{x\n\r"
                                "{cPlease choose a race, or press ENTER to display the list of races. {C[a-y]{x ", ch);
                        return FALSE;
                }

                if (ch->race < 1 || ch->race > ( MAX_RACE - 1 ) )
                {
                        send_to_char("\n\r{cPlease choose a race for your character. {C[a-y]{x ", ch);
                        return FALSE;
                }

                sprintf(buf, "\n\r{WRace selected: %s{x\n\r\n\r",
                        race_table[ch->race].race_name);
                send_to_char(buf, ch);
                do_help(ch, race_table[ch->race].race_name);
                send_to_char("\n\r{cAre you sure you want to choose this race? {C[y/n]{x ", ch);
                d->connected = CON_CONFIRM_NEW_RACE;
                break;

            case CON_CONFIRM_NEW_RACE:
                switch (argument[0])
                {
                    case 'y': case 'Y': break;
                    default:
                        send_to_char("\n\r\n\r{W}bSelect a race{x\n\r\n\r", ch);
                        do_help (ch, "race");
                        send_to_char("\n\r{cPlease choose a race for your character. {C[a-x]{x ", ch);
                        d->connected = CON_GET_NEW_RACE;
                        return FALSE;
                }

                send_to_char("\n\r\n\r{W}bSelect a gender{x\n\r\n\r"
                             "Please indicate what sex your character is.\n\r"
                             "{cMale, female or neuter? {C[m/f/n]{x ", ch);
                d->connected = CON_GET_NEW_SEX;
                break;

            case CON_GET_NEW_SEX:

                if (argument[0] == '\0')
                {
                        send_to_char("{cMale, female or neuter? {C[m/f/n]{x ", ch);
                        return FALSE;
                }

                switch (argument[0])
                {
                    case 'm': case 'M': ch->sex = SEX_MALE;    break;
                    case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
                    case 'n': case 'N': ch->sex = SEX_NEUTRAL; break;
                    default:
                        send_to_char("\n\r{Y}rInvalid gender!{x\n\r"
                                     "{cMale, female or neuter? {C[m/f/n]{x ", ch);
                        return FALSE;
                }

                d->connected = CON_DISPLAY_CLASS;
                sprintf(buf, "\n\r{WGender selected: %s{x\n\r\n\r"
                            "{cAre you sure you want this gender? {C[y/n]{x ",
                        ch->sex == SEX_MALE ? "male" :
                        (ch->sex == SEX_FEMALE ? "female" : "neuter"));
                send_to_char(buf, ch);
                break;

            case CON_DISPLAY_CLASS:

                if (argument[0] != 'y' && argument[0] != 'Y')
                {
                        send_to_char("\n\r\n\r{W}bSelect a gender{x\n\r\n\r"
                                     "Please indicate what sex your character is.\n\r"
                                     "{cMale, female or neuter? {C[m/f/n]{x ", ch);
                        d->connected = CON_GET_NEW_SEX;
                        return FALSE;
                }

                send_to_char("\n\r\n\r{W}bSelect a class{x\n\r\n\r"
                             "Now you should select which class (profession) your character belongs to.\n\r"
                             "New characters may choose one of the following classes:\n\r\n\r",
                             ch);

                for (iClass = 0; iClass < MAX_CLASS ; iClass++)
                {
                        sprintf(buf, "    %s\n\r", class_table[iClass].show_name);
                        send_to_char(buf, ch);
                }

                send_to_char("\n\r{cPlease choose a class for your character:{x ", ch);
                d->connected = CON_GET_NEW_CLASS;
                break;

            case CON_GET_NEW_CLASS:

                if (argument[0] == '\0')
                {
                        send_to_char("\n\r\n\r{W}bSelect a class{x\n\r\n\r",
                                     ch);

                        for (iClass = 0; iClass < MAX_CLASS ; iClass++)
                        {
                                sprintf(buf, "    %s\n\r", class_table[iClass].show_name);
                                send_to_char(buf, ch);
                        }

                        send_to_char("\n\r{cPlease choose a class for your character:{x ", ch);
                        return FALSE;
                }

                for (iClass = 0; iClass < MAX_CLASS ; iClass++)
                {
                        if (!str_prefix(argument, class_table[iClass].show_name))
                        {
                                ch->class = iClass;
                                break;
                        }
                }

                if (iClass == MAX_CLASS )
                {
                        send_to_char("\n\r{Y}rInvalid class!{x\n\r"
                                     "{cPlease choose a class, or press ENTER to list available classes:{x ", ch);
                        return FALSE;
                }

                write_to_buffer(d, "\n\r", 0);

                * define the classname for helps. *
                switch (ch->class)
                {
                    default: classname = "";           break;
                    case 0:  classname = "Mage";       break;
                    case 1:  classname = "Cleric";     break;
                    case 2:  classname = "Thief";      break;
                    case 3:  classname = "Warrior";    break;
                    case 4:  classname = "Psionicist"; break;
                    case 5:  classname = "Shapeshifter"; break;
                    case 6:  classname = "Brawler";     break;
                    case 7:  classname = "Ranger";      break;
                    case 8:  classname = "Smithy";      break;
                }

                if (strlen(classname) > 0)
                {
                        sprintf(buf, "\n\r{WClass selected: %s{x\n\r\n\r",
                                classname);
                        send_to_char(buf, ch);
                        do_help(ch, classname);
                }
                else
                        bug("Nanny CON_GET_NEW_CLASS:  ch->class (%d) not valid", ch->class);

                send_to_char("\n\r{cAre you sure you want this class? {C[y/n]{x ", ch);
                d->connected = CON_CONFIRM_CLASS;
                break;

            case CON_CONFIRM_CLASS:
                switch (argument[0])
                {
                    case 'y': case 'Y': break;
                    default:
                        send_to_char("\n\r\n\r{W}bSelect a class{x\n\r\n\r", ch);

                        for (iClass = 0; iClass < MAX_CLASS ; iClass++)
                        {
                                sprintf(buf, "    %s\n\r", class_table[iClass].show_name);
                                send_to_char(buf, ch);
                        }

                        send_to_char("\n\r{cPlease choose a class for your character:{x ", ch);
                        d->connected = CON_GET_NEW_CLASS;
                        return FALSE;
                }

                sprintf(log_buf, "%s@%s new player.", ch->name, d->host);
                log_string(log_buf);

                send_to_char("\n\r\n\r{W}bGenerate random attribute scores{x\n\r\n\r", ch);
                do_help(ch, "random");
                send_to_char("\n\r{cPress {CENTER{X{c to begin rolling your character's attributes.{x ", ch);
                d->connected = CON_GENERATE_STATS;
                break;

            case CON_GENERATE_STATS:
                generate_stats (ch);

                sprintf(buf, "\n\rStr: %2d  Int: %2d  Wis: %2d  Dex: %2d  Con: %2d\n\r"
                        "{cAccept? {C[y/n]{x ",
                        ch->pcdata->perm_str,
                        ch->pcdata->perm_int,
                        ch->pcdata->perm_wis,
                        ch->pcdata->perm_dex,
                        ch->pcdata->perm_con);
                send_to_char(buf, ch);
                d->connected = CON_CONFIRM_STATS;
                break;

            case CON_CONFIRM_STATS:
                switch (argument[0])
                {
                    case 'y': case 'Y': break;
                    default:
                        generate_stats (ch);

                        sprintf(buf, "\n\rStr: %2d  Int: %2d  Wis: %2d  Dex: %2d  Con: %2d\n\r"
                                "{cAccept? {C[y/n]{x ",
                                ch->pcdata->perm_str,
                                ch->pcdata->perm_int,
                                ch->pcdata->perm_wis,
                                ch->pcdata->perm_dex,
                                ch->pcdata->perm_con);
                        send_to_char(buf, ch);
                        return FALSE;
                }

                send_to_char("\n\r{WCharacter generation complete.{x\n\r\n\r"
                             "You are now ready to enter the Dragons Domain and begin your training!\n\r\n\r", ch);
                SET_BIT(ch->act, PLR_AUTOLEVEL);
                ch->pcdata->pagelen = 100;
                d->connected = CON_READ_MOTD;
                break;

            case CON_READ_MOTD:
                SET_BIT(ch->act, PLR_AUTOEXIT);
                ch->next = char_list;
                char_list = ch;
                d->connected = CON_PLAYING;

                send_to_char("\n\r{RWelcome to the Dragons Domain... tread carefully, adventurer!{x\n\r", ch);

                if (ch->level == 0)
                {
                        OBJ_DATA *obj;

                        ch->level = 1;
                        ch->exp = 1000;

                        * give the player the base knowledge - geoff *
                        ch->pcdata->learned[gsn_mage_base + ch->class] = 30;

                        * set initial train to prime req *
                        ch->pcdata->stat_train = class_table[ch->class].attr_prime;

                        ch->pcdata->learned[skill_lookup(race_table[ch->race].spell_one)] = 99;
                        ch->pcdata->learned[skill_lookup(race_table[ch->race].spell_two)] = 99;

                        startcoinsamt   = 50 + number_fuzzy(3)
                                * number_fuzzy(4) * number_fuzzy(5) * 9;

                        coin_crunch (startcoinsamt, startcoins);
                        ch->plat = startcoins->plat;
                        ch->gold = startcoins->gold;
                        ch->silver = startcoins->silver;
                        ch->copper = startcoins->copper;
                        free (startcoins);

                        *
                         * Shade 17.6.22
                         *
                         * Make it easier for starting characters; will edit here in the level 0 code not in clear_char
                         *
                         * Give them a few of each pracs to get going, make starting hits 50 not 20
                         *


                        ch->max_hit = 50;
                        ch->hit = ch->max_hit;

                        ch->pcdata->str_prac = 2;
                        ch->pcdata->int_prac = 2;

                        if (ch->class == CLASS_CLERIC || ch->class == CLASS_MAGE || ch->class == CLASS_PSIONICIST)
                        {
                                ch->pcdata->int_prac += 3;
                                ch->max_mana = 150;
                        }

                        ch->mana        = ch->max_mana;
                        ch->move        = ch->max_move;

                        set_title(ch, " is a newbie");
                        free_string(ch->prompt);
                        ch->prompt = str_dup("<<{G%h/%H{x hits {C%m/%M{x mana {Y%v/%V{x move [{W%z{x]> ");

                        obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0);
                        obj_to_char(obj, ch);
                        equip_char(ch, obj, WEAR_LIGHT);

                        obj = create_object(get_obj_index(OBJ_VNUM_POUCH), 0);
                        obj_to_char(obj, ch);
                        equip_char(ch, obj, WEAR_POUCH);

                        obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0);
                        obj_to_char(obj, ch);
                        equip_char(ch, obj, WEAR_BODY);

                        if (ch->class != CLASS_THIEF
                            && ch->class != CLASS_BRAWLER
                            && ch->class != CLASS_MAGE)
                        {
                                obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0);
                                obj_to_char(obj, ch);
                                equip_char(ch, obj, WEAR_SHIELD);
                        }

                        if (ch->class != CLASS_BRAWLER)
                        {
                                obj = create_object( get_obj_index(class_table[ch->class].weapon), 0);
                                obj_to_char(obj, ch);
                                equip_char(ch, obj, WEAR_WIELD);
                                SET_BIT(ch->act, PLR_AUTOWIELD);
                        }

                        if (ch->class == CLASS_RANGER)
                        {
                                obj = create_object (get_obj_index(OBJ_VNUM_SCHOOL_BOW), 0);
                                obj_to_char(obj, ch);
                                equip_char(ch, obj, WEAR_RANGED_WEAPON);
                        }

                        char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));
                }

                else if (ch->in_room)
                {
                        char_to_room(ch, ch->in_room);
                }

                else
                {
                        char_to_room(ch, get_room_index(DEFAULT_RECALL));
                }

                if (ch->level == 1)
                {
                        sprintf(buf,"%s, a %s of the %s race, is born unto the Domain.",
                                ch->name,
                                full_class_name(ch->class),
                                race_table[ch->race].race_name);
                        do_info(ch, buf);
                }

                if (!IS_SET(ch->act, PLR_WIZINVIS) && ch->level > 1)
                {
                        sprintf(buf, "%s has entered the Dragons Domain...", ch->name);
                        do_info(ch,  buf);
                }

                *
                 *  Entry lag and warning; Gez & Shade 2000
                 *
                if (ch->level <= LEVEL_HERO)
                {
                        for (d = descriptor_list; d; d = d->next)
                        {
                                if (d->connected == CON_PLAYING
                                    && d->character
                                    && d->character != ch)
                                {
                                        if (d->character->in_room == ch->in_room)
                                        {
                                                sprintf(buf, "\n\r{R%s has appeared in the room!{x\n\r\n\r",
                                                        ch->name);
                                                send_to_char(buf, d->character);
                                        }
                                        else if (d->character->in_room->area == ch->in_room->area)
                                        {
                                                sprintf(buf, "\n\r{R%s has appeared in the area!{x\n\r\n\r",
                                                        ch->name);
                                                send_to_char(buf, d->character);
                                        }
                                }
                        }
                }

                if (IS_SET(ch->act, PLR_QUESTOR))
                {
                        REMOVE_BIT(ch->act, PLR_QUESTOR);
                        ch->pcdata->questgiver = NULL;
                        ch->pcdata->countdown  = 0;
                        ch->pcdata->questmob   = 0;
                        ch->pcdata->questobj   = 0;
                }

                if (ch->level > 1)
                        do_board (ch, "");

                send_to_char( "\n\r", ch );
                do_look(ch, "auto");

                *  Entry lag  *
                if (ch->level <= LEVEL_HERO)
                {
                        send_to_char("\n\r{RYou must take a few moments to adjust to your new surroundings...{x\n\r", ch);
                        WAIT_STATE(ch, PULSE_VIOLENCE);
                }

                if (ch->silent_mode)
                        send_to_char("\n\rYou are in silent mode.\n\r", ch);

                break;

            case CON_NOTE_TO:
                handle_con_note_to (d, argument);
                break;

            case CON_NOTE_SUBJECT:
                handle_con_note_subject (d, argument);
                break;

            case CON_NOTE_EXPIRE:
                handle_con_note_expire (d, argument);
                break;

            case CON_NOTE_TEXT:
                handle_con_note_text (d, argument);
                break;

            case CON_NOTE_FINISH:
                handle_con_note_finish (d, argument);
                break;

            case CON_GET_DISCONNECTION_PASSWORD:
#if defined(unix)
                write_to_buffer(d, "\n\r", 2);
#endif
                if (ch->pcdata->pwd != NULL
                    && strlen(ch->pcdata->pwd) > 0
                    && strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
                {
                        close_socket(d);
                        return FALSE;
                }

                send_echo_on(d);
                * write_to_buffer(d, echo_on_str, 0); *

                for (temp = descriptor_list; temp; temp = temp->next)
                {
                        if (temp != d
                            && temp->character
                            && temp->connected != CON_GET_NAME
                            && temp->connected != CON_GET_DISCONNECTION_PASSWORD
                            && !str_cmp(temp->original ? temp->original->name
                                        : temp->character->name, ch->name)
                            && !temp->character->deleted)
                        {
                                write_to_buffer(temp, "Another player has disconnected you.\n\r", 0);
                                free_char(d->character);
                                d->character = temp->character;
                                d->character->desc = d;
                                close_socket(temp);
                        }
                }

                sprintf(log_buf, "%s@%s has reconnected themselves",
                        d->character->name,
                        d->host);
                log_string(log_buf);
                send_to_char("Reconnecting...\n\r", d->character);
                act("$n has reconnected.", d->character, NULL, NULL, TO_ROOM);
                d->connected = CON_PLAYING;
                break;
        }
        pop_call();
	return FALSE;
} * nanny mark *
*/

/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.       
 */
void send_to_char(const char *txt, CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	bool foundyou;

	push_call("send_to_char(%p,%p)",txt,ch);

	if (txt == NULL || ch == NULL || ch->desc == NULL)
	{
		pop_call();
		return;
	}

	if (IS_SET(mud->flags, MUD_SKIPOUTPUT))
	{
		if (IS_NPC(ch) || ch->level < MAX_LEVEL /*|| !IS_SET(ch->act, PLR_WIZTIME)*/)
		{
			pop_call();
			return;
		}
	}

	if (txt[0] != '\033')
	{
		foundyou = scroll_you(ch->desc, txt, IS_SET(CH(ch->desc)->pcdata->vt100_flags, VT100_HIGHLIGHT));

		if (foundyou)
		{
			snprintf(buf, MAX_STRING_LENGTH-1, "%s%s", get_color_string(ch, COLOR_TEXT, VT102_BOLD), txt);
		}
		else
		{
			snprintf(buf, MAX_STRING_LENGTH-1, "%s%s", get_color_string(ch, COLOR_TEXT, VT102_DIM),  txt);
		}
		write_to_buffer(ch->desc, (char *)buf, 0);
	}
	else
	{
		write_to_buffer(ch->desc, (char *)txt, 0);
	}
	pop_call();
	return;
}

/*
void send_to_char (const char *txt, CHAR_DATA *ch)
{
        const char    *point;
        char    *point2;
        char    buf[ MAX_STRING_LENGTH*4 ];
        int     skip = 0;
        char cur_code[3];
        int cd_count = 2;
        int cv_int;
        char open_token;
        char close_token;

        * If you change these tokens, you should change the color_table_8bit act_codes as well *
        open_token   = '<';
        close_token  = '>';

        buf[0] = '\0';
        point2 = buf;

        if (txt && ch->desc)
        {
                if (IS_SET(ch->act, PLR_ANSI))
                {
                        for(point = txt ; *point ; point++)
                        {
                                if(*point == '{')
                                {
                                        point++;
                                        skip = colour(*point, ch, point2);
                                        while(skip-- > 0)
                                                ++point2;
                                        continue;
                                }

                                if(*point == '}')
                                {
                                        point++;
                                        skip = bgcolour(*point, ch, point2);
                                        while(skip-- > 0)
                                                ++point2;
                                        continue;
                                }

                                if (*point == open_token)
                                {
                                        point++;

                                        if (*point == open_token)
                                        {

                                                *point2 = *point;
                                                *++point2 = '\0';
                                                continue;
                                        }

                                        if (isdigit(*point))
                                        {
                                                if (cd_count < 0)
                                                        continue;

                                                cur_code[cd_count] = *point;
                                                cd_count--;
                                                point++;

                                                if (isdigit(*point))
                                                {
                                                        if (cd_count < 0)
                                                                continue;

                                                        cur_code[cd_count] = *point;
                                                        cd_count--;
                                                        point++;

                                                        if (isdigit(*point))
                                                        {
                                                                if (cd_count < 0)
                                                                        continue;

                                                                cur_code[cd_count] = *point;
                                                                cd_count--;
                                                                point++;
                                                        }
                                                }
                                        }
                                        else {
                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;
                                                continue;
                                        }

                                        if (*point == close_token)
                                        {

                                                reverse_char_array(cur_code, 3);

                                                cv_int = atoi( cur_code );


                                                skip = colour_8bit(cv_int, ch, point2);

                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;

                                                while (skip-- > 0)
                                                        ++point2;

                                                continue;
                                        }
                                        else {

                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;

                                                continue;
                                        }
                                }

                                *point2 = *point;
                                *++point2 = '\0';
                        }
                        *point2 = '\0';
                        free_string(ch->desc->showstr_head);
                        ch->desc->showstr_head  = str_dup(buf);
                        ch->desc->showstr_point = ch->desc->showstr_head;
                        show_string(ch->desc, "");
                }
                else
                {

                        cur_code[0] = '\0';
                        cur_code[1] = '\0';
                        cur_code[2] = '\0';
                        cd_count = 2;

                        for(point = txt ; *point ; point++)
                        {
                                if(*point == '{')
                                {
                                        point++;
                                        if(*point == '{')
                                        {
                                                *point2 = *point;
                                                *++point2 = '\0';
                                        }
                                        continue;
                                }

                                if(*point == '}')
                                {
                                        point++;
                                        if(*point == '}')
                                        {
                                                *point2 = *point;
                                                *++point2 = '\0';
                                        }
                                        continue;
                                }

                                if (*point == open_token)
                                {
                                        point++;

                                        if (*point == open_token)
                                        {

                                                *point2 = *point;
                                                *++point2 = '\0';
                                                continue;
                                        }

                                        if (isdigit(*point))
                                        {
                                                if (cd_count < 0)
                                                        continue;

                                                cur_code[cd_count] = *point;
                                                cd_count--;
                                                point++;

                                                if (isdigit(*point))
                                                {
                                                        if (cd_count < 0)
                                                                continue;

                                                        cur_code[cd_count] = *point;
                                                        cd_count--;
                                                        point++;

                                                        if (isdigit(*point))
                                                        {
                                                                if (cd_count < 0)
                                                                        continue;

                                                                cur_code[cd_count] = *point;
                                                                cd_count--;
                                                                point++;
                                                        }
                                                }
                                        }
                                        else {
                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;
                                                continue;
                                        }

                                        if (*point == close_token)
                                        {
                                                reverse_char_array(cur_code, 3);
                                                cv_int = atoi( cur_code );
                                                strip_colour_8bit(cv_int, ch, point2);

                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;

                                                continue;
                                        }
                                        else {
                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;

                                                continue;
                                        }
                                }


                                *point2 = *point;
                                *++point2 = '\0';
                        }
                        *point2 = '\0';
                        free_string(ch->desc->showstr_head);
                        ch->desc->showstr_head  = str_dup(buf);
                        ch->desc->showstr_point = ch->desc->showstr_head;
                        show_string(ch->desc, "");
                }
        }
        return;
}
*/

bool scroll_you(DESCRIPTOR_DATA *d, const char *txi, bool youcheck)
{
	register char *pti;

	push_call("scroll_you(%p,%p,%p)",d,txi,youcheck);

	if (!youcheck)
	{
		pop_call();
		return(FALSE);
	}

	if (!IS_SET(CH(d)->pcdata->vt100_flags, VT100_BOLD))
	{
		pop_call();
		return FALSE;
	}

	if (d->connected != CON_PLAYING)
	{
		pop_call();
		return(FALSE);
	}

	if (strlen(txi) < 3)
	{
		pop_call();
		return FALSE;
	}

	for (pti = (char *) txi ; *(pti+2) != '\0' ; pti++)
	{
		if ((*(pti+0) == 'Y' || *(pti+0) == 'y')
		&&  (*(pti+1) == 'O' || *(pti+1) == 'o')
		&&  (*(pti+2) == 'U' || *(pti+2) == 'u'))
		{
			pop_call();
			return TRUE;
		}
	}
	pop_call();
	return FALSE;
}

/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.       
 */
bool check_parse_name(char *name , bool mobcheck)
{
	push_call("check_parse_name(%p,%p)",name,mobcheck);

	if (is_name(name, "new"))
	{
		pop_call();
		return TRUE;
	}

	/*	Reserved Words	*/

	if (is_name(name, "all auto immortal self god enemy clan target race class someone north east south west down open close hours trivia morph hit mana armor damage castle save bak del dmp hours"))
	{
		pop_call();
		return FALSE;
	}

	/*	Length restrictions.*/

	if (strlen(name) <  3)
	{
		pop_call();
		return FALSE;
	}

	if (strlen(name) > 12)
	{
		pop_call();
		return FALSE;
	}

	/*
		Alphanumerics only.
	*/

	{
		char *pc;

		for (pc = name ; *pc != '\0' ; pc++)
		{
			if ((*pc < 'a' || *pc > 'z') && (*pc < 'A' || *pc > 'Z'))
			{
				pop_call();
				return FALSE;
			}
		}

		/*
			No more names containing 'you' - Scandum 16-05-2002
		*/

		for (pc = name ; *(pc+2) != '\0' ; pc++)
		{
			if ((*(pc+0) == 'Y' || *(pc+0) == 'y')
			&&  (*(pc+1) == 'O' || *(pc+1) == 'o')
			&&  (*(pc+2) == 'U' || *(pc+2) == 'u'))
			{
				pop_call();
				return FALSE;
			}
		}
	}

	/*
		Don't allow players to name themself after mobs unless you
		make changes to mob_prog.c - Scandum
	*/

	if (mobcheck)
	{
		extern MOB_INDEX_DATA *mob_index_hash [ MAX_KEY_HASH ];
                MOB_INDEX_DATA *pMobIndex;
                int iHash;

                for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
                {
                        for (pMobIndex  = mob_index_hash[iHash];
                             pMobIndex;
                             pMobIndex  = pMobIndex->next)
                        {
                                if (is_name(name, pMobIndex->player_name))
                                        return FALSE;
                        }
                }
	}
	pop_call();
	return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect (DESCRIPTOR_DATA *d, char *name, bool fConn)
{
        /* OBJ_DATA  *obj; */
        CHAR_DATA *ch;
        char buf[MAX_STRING_LENGTH];

        for (ch = char_list; ch; ch = ch->next)
        {
                if (ch->deleted)
                        continue;

                if (!IS_NPC(ch)
                    && (!fConn || !ch->desc)
                    && !str_cmp(d->character->name, ch->name))
                {
                        if (fConn == FALSE)
                        {
                                free_string(d->character->pcdata->pwd);
                                d->character->pcdata->pwd = str_dup(ch->pcdata->pwd);
                        }
                        else
                        {
                                free_char(d->character);
                                d->character = ch;
                                ch->desc     = d;
                                ch->timer    = 0;
                                send_to_char("Reconnecting.\n\r", ch);
                                act("$n has reconnected.", ch, NULL, NULL, TO_ROOM);
                                sprintf(log_buf, "%s@%s reconnected (to desc %d).",
                                        ch->name, d->host, d->descriptor);
                                log_string(log_buf);
                                d->connected = CON_PLAYING;

                                /*
                                 *  Reconnection must not be able to be used to
                                 *  avoid entry lag; Gez 2000
                                 */
                                if (ch->level <= LEVEL_HERO)
                                {
                                        for (d = descriptor_list; d; d = d->next)
                                        {
                                                if (d->connected == CON_PLAYING
                                                    && d->character
                                                    && d->character != ch)
                                                {
                                                        if (d->character->in_room == ch->in_room)
                                                        {
                                                                sprintf(buf, "\n\r{R%s has appeared in the room!{x\n\r\n\r",
                                                                        ch->name);
                                                                send_to_char(buf, d->character);
                                                        }
                                                        else if ((ch->was_in_room
                                                                  && d->character->in_room->area == ch->was_in_room->area)
                                                                 || d->character->in_room->area == ch->in_room->area)
                                                        {
                                                                sprintf(buf, "\n\r{R%s has appeared in the area!{x\n\r\n\r",
                                                                        ch->name);
                                                                send_to_char(buf, d->character);
                                                        }
                                                }
                                        }
                                        send_to_char("\n\r{RYou must take a few moments to adjust to your new surroundings...{x\n\r", ch);
                                        WAIT_STATE(ch, PULSE_VIOLENCE);
                                }

                                if (ch->pcdata->in_progress)
                                        send_to_char ("You have a note in progress. Type NWRITE to continue it.\n\r",ch);

                                /*
                                 * Contributed by Gene Choi
                                 *
                                if ((obj = get_eq_char(ch, WEAR_LIGHT))
                                    && obj->item_type == ITEM_LIGHT
                                    && obj->value[2] != 0
                                    && ch->in_room)
                                        ++ch->in_room->light;
                                 */
                        }
                        return TRUE;
                }
        }

        return FALSE;
}


/*
 * Check if already playing.
 *
 * Now handles players reconnecting themselves.
 */
bool check_playing (DESCRIPTOR_DATA *d, char *name)
{
        DESCRIPTOR_DATA *dold;

        for (dold = descriptor_list; dold; dold = dold->next)
        {
                if (dold != d
                    && dold->character
                    && dold->connected != CON_GET_NAME
                    && !str_cmp(name, dold->original
                                ? dold->original->name : dold->character->name)
                    && !dold->character->deleted)
                {
                        write_to_buffer(d, "That character is already playing.\n\r", 0);

                        /* Temporarily out of action */
                        return TRUE;

                        if (dold->connected != CON_PLAYING)
                        {
                                write_to_buffer(d, "You may not safely disconnect them at the moment.\n\r", 0);
                                return TRUE;
                        }

                        write_to_buffer(d, "To reconnect using that character, enter their "
                                        "password.\n\rPassword for reconnection: ", 1000000);
                        /* write_to_buffer(d, echo_off_str, 0); */
                        send_echo_off(d);
                        d->connected = CON_GET_DISCONNECTION_PASSWORD;

                        return TRUE;
                }
        }

        return FALSE;
}

/*

void stop_idling(DESCRIPTOR_DATA *d)
{
	push_call("stop_idling(%p)",d);

	if (d->character == NULL || d->connected < CON_PLAYING)
	{
		pop_call();
		return;
	}

        if ( !CH(d)->desc
            || !CH(d)->was_in_room
            ||  CH(d)->in_room != get_room_index(ROOM_VNUM_LIMBO))
        {
                		pop_call();
                                return;
        }
	CH(d)->timer = 0;
        char_from_room(CH(d));
        char_to_room(CH(d), CH(d)->was_in_room);
        CH(d)->was_in_room = NULL;
        act("$n has returned from the void.", CH(d), NULL, NULL, TO_ROOM);
        return;


	pop_call();
	return;
}
*/

void stop_idling(DESCRIPTOR_DATA *d)
{
	push_call("stop_idling(%p)",d);

	if (d->character == NULL || d->connected >= CON_PLAYING)
	{
		pop_call();
		return;
	}

	if (IS_SET(CH(d)->pcdata->interp, INTERP_AUTO))
	{
		pop_call();
		return;
	}

	CH(d)->timer = 0;

	if (CH(d)->in_room != get_room_index(ROOM_VNUM_LIMBO))
	{
		pop_call();
		return;
	}

	/* if (CH(d)->level < LEVEL_IMMORTAL && room_is_private(room_index[CH(d)->pcdata->was_in_room]))
	{
		char_to_room(CH(d), ROOM_VNUM_TEMPLE);
		send_to_char("The room you were idling in is now private.\n\r", CH(d));
	}
	else */
	{
		char_to_room(CH(d), CH(d)->pcdata->was_in_room);
	}

	show_who_can_see(CH(d), " has returned from the void.\n\r");

	pop_call();
	return;
}

bool is_desc_valid(CHAR_DATA *ch)
{
	push_call("is_desc_valid(%p)",ch);

	if (ch == NULL)
	{
		pop_call();
		return(FALSE);
	}

	if (ch->desc != NULL && ch->desc->character == ch)
	{
		pop_call();
		return(TRUE);
	}
	else
	{
		pop_call();
		return(FALSE);
	}
}

/*
void stop_idling(CHAR_DATA *ch)
{
        if (!ch
            || !ch->desc
            ||  ch->desc->connected != CON_PLAYING
            || !ch->was_in_room
            ||  ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
        {
                return;
        }

        ch->timer = 0;
        char_from_room(ch);
        char_to_room(ch, ch->was_in_room);
        ch->was_in_room = NULL;
        act("$n has returned from the void.", ch, NULL, NULL, TO_ROOM);
        return;
}
*/

/*
 * Write to all in the room.
 */
void send_to_room (const char *txt, ROOM_INDEX_DATA *room)
{
        DESCRIPTOR_DATA *d;

        for (d = descriptor_list; d; d = d->next)
                if (d->character != NULL)
                        if (d->character->in_room == room)
                                act(txt, d->character, NULL, NULL, TO_CHAR);
}


/*
 * Write to all characters.
 */
void send_to_all_char (const char *text)
{
        DESCRIPTOR_DATA *d;

        if (!text)
                return;

        for (d = descriptor_list; d; d = d->next)
                if (d->connected == CON_PLAYING)
                        send_to_char(text, d->character);

        return;
}


/*
 * Write to one char.
 */
void send_to_char_bw (const char *txt, CHAR_DATA *ch)
{
        if(!txt || !ch->desc)
                return;

        /*
         * Bypass the paging procedure if the text output is small
         * Saves process time.
         */
        if (strlen(txt) < 600)
                write_to_buffer(ch->desc, (char *)txt, strlen(txt));
        else
        {
                free_string(ch->desc->showstr_head);
                ch->desc->showstr_head  = str_dup(txt);
                ch->desc->showstr_point = ch->desc->showstr_head;
                show_string(ch->desc, "");
        }

        return;
}


void show_string (struct descriptor_data *d, char *input)
{
        register char *scan;
        char  buffer[ MAX_STRING_LENGTH*6 ];
        char  buf   [ MAX_INPUT_LENGTH    ];
        int   line      = 0;
        int   toggle    = 0;
        int   pagelines = 20;

        one_argument(input, buf);

        switch(UPPER(buf[0]))
        {
            case '\0':
            case 'C': /* show next page of text */
                break;

            case 'R': /* refresh current page of text */
                toggle = 1;
                break;

            case 'B': /* scroll back a page of text */
                toggle = 2;
                break;

            default: /*otherwise, stop the text viewing */
                if (d->showstr_head)
                {
                        free_string(d->showstr_head);
                        d->showstr_head = str_dup("");
                }
                d->showstr_point = 0;
                return;
        }

        if (d->original)
                pagelines = d->original->pcdata->pagelen;
        else
                pagelines = d->character->pcdata->pagelen;

        if (toggle)
        {
                if (d->showstr_point == d->showstr_head)
                        return;
                if (toggle == 1)
                        line = -1;
                do
                {
                        if (*d->showstr_point == '\n')
                                if ((line++) == (pagelines * toggle))
                                        break;
                        d->showstr_point--;
                }
                while (d->showstr_point != d->showstr_head);
        }

        line    = 0;
        *buffer = 0;
        scan    = buffer;
        if (*d->showstr_point)
        {
                do
                {
                        *scan = *d->showstr_point;
                        if (*scan == '\n')
                                if ((line++) == pagelines)
                                {
                                        scan++;
                                        break;
                                }
                        scan++;
                        d->showstr_point++;
                        if(*d->showstr_point == 0)
                                break;
                }
                while(1);
        }

        *scan = 0;

        write_to_buffer(d, buffer, strlen(buffer));
        if (*d->showstr_point == 0)
        {
                free_string(d->showstr_head);
                d->showstr_head  = str_dup("");
                d->showstr_point = 0;
        }

        return;
}


void ansi_color(const char *txt, CHAR_DATA *ch)
{
        if (txt != NULL && ch->desc != NULL)
        {
                if (!IS_SET(ch->act,PLR_ANSI) && !IS_SET(ch->act,PLR_VT100))
                        return;
                else
                        if (IS_SET(ch->act,PLR_VT100) &&  !IS_SET(ch->act,PLR_ANSI))
                        {
                                if (!str_cmp(txt, GREEN)
                                    || !str_cmp(txt, RED)
                                    || !str_cmp(txt, BLUE)
                                    || !str_cmp(txt, BLACK)
                                    || !str_cmp(txt, CYAN)
                                    || !str_cmp(txt, GREY)
                                    || !str_cmp(txt, YELLOW)
                                    || !str_cmp(txt, PURPLE))
                                        return;
                        }
                write_to_buffer(ch->desc, (char *)txt, strlen(txt));
                return;
        }
}


/*
 * The primary output interface for formatted output.
 */
/**
 * REPLACED with LOLA code Lola  1.0 by Igor van den Hoven.   
 * Deal with sockets that haven't logged in yet.    
 */
void act(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type)
{
	static char * const he_she  [] = { "it",  "he",  "she" };
	static char * const him_her [] = { "it",  "him", "her" };
	static char * const his_her [] = { "its", "his", "her" };
/*
	static char * const fake_name [] =
	{
		"no one in particular",		"a white rabbit",
		"Puff",					"a house plant",
		"a twinkie",				"your mother",
		"your alter ego",			"a buffer overflow",
		"a voice in your head",		"the wind",
		"Bubba",					"your father",
		"a lollipop",				"your mud client",
		"your index finger",		"a large worm",
		"the ground",				"a bad pointer reference",
		"your alter ego",			"a fortune cookie",
		"your shadow",				"the world"
	};
*/
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *to;
	CHAR_DATA *vch = (CHAR_DATA *) arg2;
	OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
	OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
	const char *str;
	const char *i;
	char *point;

	push_call("act(%p,%p,%p,%p,%p)",format,ch,arg1,arg2,type);

	/*
		Discard null messages generated by socials.
	*/

	if (format == NULL || format[0] == '\0')
	{
		pop_call();
		return;
	}

	if (IS_SET(mud->flags, MUD_SKIPOUTPUT))
	{
		if (IS_NPC(ch) || ch->level < MAX_LEVEL /* || !IS_SET(ch->act, PLR_WIZTIME)*/)
		{
			pop_call();
			return;
		}
	}

	if (ch == NULL)
	{
		bug("Act: null ch.", 0);
		pop_call();
		return;
	}

	to = ch->in_room->first_person;

	if (type == TO_VICT)
	{
		if (vch == NULL)
		{
			bug("Act: null vch with TO_VICT.", 0);
			pop_call();
			return;
		}
		if (vch->in_room == NULL)
		{
			bug("Act: null victim room.", 0);
			pop_call();
			return;
		}
		to = vch->in_room->first_person;
	}

	for (; to != NULL ; to = to->next_in_room)
	{
		if (to->desc == NULL || !IS_AWAKE(to))
		{
			continue;
		}
		if (type == TO_CHAR && to != ch)
		{
			continue;
		}
		if (type == TO_VICT && (to != vch || to == ch))
		{
			continue;
		}
		if (type == TO_ROOM && to == ch)
		{
			continue;
		}
		if (type == TO_NOTVICT && (to == ch || to == vch))
		{
			continue;
		}

		point = buf;
		str = format;

		while (*str != '\0')
		{
			if (*str != '$')
			{
				*point++ = *str++;
				continue;
			}
			++str;

			if (arg1 == NULL && *str == 'p')
			{
				log_printf("Act: missing arg1 for code %d.", *str);
				i = "<ppp>";
			}
			else if (arg2 == NULL && *str >= 'A' && *str <= 'Z')
			{
				log_printf("Act: missing arg2 for code %d.", *str);
				i = "<A-Z>";
			}

	/*		else	if (!IS_AFFECTED(to, AFF2_HALLUCINATE))
			{ */
			switch (*str)
                        {
                                default:
                                        log_printf("Act: act bad code %d.", *str);
                                        i = "<? ?>";
                                        break;
                                case 't':
                                        i = (char *) arg1;
                                        break;
                                case 'T':
                                        i = (char *) arg2;
                                        break;
                                case 'n':
                                        i = get_name(ch);
                                        break;
                                case 'N':
                                        i = get_name(vch);
                                        break;
                                case 'e':
                                        i = he_she  [URANGE(0, ch->sex,  2)];
                                        break;
                                case 'E':
                                        i = he_she  [URANGE(0, vch->sex, 2)];
                                        break;
                                case 'm':
                                        i = him_her [URANGE(0, ch->sex,  2)];
                                        break;
                                case 'M':
                                        i = him_her [URANGE(0, vch->sex, 2)];
                                        break;
                                case 's':
                                        i = his_her [URANGE(0, ch->sex,  2)];
                                        break;
                                case 'S':
                                        i = his_her [URANGE(0, vch->sex, 2)];
                                        break;
                                case 'p':
                                        i = can_see_obj(to, obj1) ? obj1->short_descr : "something";
                                        break;
                                case 'P':
                                        i = can_see_obj(to, obj2) ? obj2->short_descr : "something";
                                        break;
                                case 'd':
                                        if (arg2 == NULL || ((char *) arg2)[0] == '\0')
                                        {
                                                i = "door";
                                        }
                                        else
                                        {
                                                i = arg2;
                                        }
                                        break;
                                case '/':
                                        i = "\n\r";
                                        break;
                        }
			/*}
			 else  THIS IS THE HALLUCINATE VERSION...DON'T PANIC -ORDER 
			{
				switch (*str)
				{
					default:
						log_printf("Act: act bad code %d.", *str);
						i = ">? ?<";
						break;
					case 't':
					case 'T':
					case 'n':
					case 'N':
					case 'p':
					case 'P':
					case 'd':
						i = fake_name[number_range(0, NUM_FAKE_NAME-1)];
						break;
					case 'e':
					case 'E':
						i = he_she  [number_range(0, 2)];
						break;
					case 'm':
					case 'M':
						i = him_her [number_range(0, 2)];
						break;
					case 's':
					case 'S':
						i = his_her [number_range(0, 2)];
						break;
				}
			} 
			if (i == NULL)
			{
				log_string("hallucinate act: i == NULL");
				dump_stack();
				i = "nothing";
			} */
			++str;
			while ((*point = *i) != '\0')
			{
				++point, ++i;
			}
		}
		*point++ = '\n';
		*point++ = '\r';
		*point++ = '\0';

		strcpy(buf, capitalize(buf));

		send_to_char_color(ansi_justify(buf, get_page_width(to)), to);
	}

	/*
		Placed act_prog triggering in a seperate loop. This way the act prog
		triggers after the message has been send. - Scandum
	*/

	if (type == TO_VICT)
	{
		to = vch->in_room->first_person;
	}
	else
	{
		to = ch->in_room->first_person;
	}

	for (; to ; to = to->next_in_room)
	{
		if (!MP_VALID_MOB(to) || !IS_SET(to->pIndexData->progtypes, ACT_PROG) || !IS_AWAKE(to))
		{
			continue;
		}
		if (type == TO_CHAR && to != ch)
		{
			continue;
		}
		if (type == TO_VICT && (to != vch || to == ch))
		{
			continue;
		}
		if (type == TO_ROOM && to == ch)
		{
			continue;
		}
		if (type == TO_NOTVICT && (to == ch || to == vch))
		{
			continue;
		}

		point = buf;
		str = format;
		while (*str != '\0')
		{
			if (*str != '$')
			{
				*point++ = *str++;
				continue;
			}
			++str;

			if (arg1 == NULL && *str == 'p')
			{
				log_printf("Act: missing arg1 for code %d.", *str);
				i = "<ppp>";
			}
			else if (arg2 == NULL && *str >= 'A' && *str <= 'Z')
			{
				log_printf("Act: missing arg2 for code %d.", *str);
				i = "<A-Z>";
			}

			switch (*str)
			{
				default:
					log_printf("Act: act bad code %d.", *str);
					i = "<? ?>";
					break;
				case 't':
					i = (char *) arg1;
					break;
				case 'T':
					i = (char *) arg2;
					break;
				case 'n':
					i = get_name(ch);
					break;
				case 'N':
					i = get_name(vch);
					break;
				case 'e':
					i = he_she  [URANGE(0, ch->sex,  2)];
					break;
				case 'E':
					i = he_she  [URANGE(0, vch->sex, 2)];
					break;
				case 'm':
					i = him_her [URANGE(0, ch->sex,  2)];
					break;
				case 'M':
					i = him_her [URANGE(0, vch->sex, 2)];
					break;
				case 's':
					i = his_her [URANGE(0, ch->sex,  2)];
					break;
				case 'S':
					i = his_her [URANGE(0, vch->sex, 2)];
					break;
				case 'p':
					i = obj1->short_descr;
					break;
				case 'P':
					i = obj2->short_descr;
					break;
				case 'd':
					if (arg2 == NULL || ((char *) arg2)[0] == '\0')
					{
						i = "door";
					}
					else
					{
						i = arg2;
					}
					break;
				case '/':
					i = "\n\r";
					break;
			}

			if (i == NULL)
			{
				log_string("act: i == NULL");
				i = "nothing";
			}
			++str;
			while ((*point = *i) != '\0')
			{
				++point, ++i;
			}
		}
		*point++ = '\n';
		*point++ = '\r';
		*point++ = '\0';

		buf[0]   = UPPER(buf[0]);

		mprog_act_trigger(buf, to, ch, obj1, vch);
	}
	pop_call();
	return;
}

/* void act (const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type)
{
        OBJ_DATA        *obj1        = (OBJ_DATA  *) arg1;
        OBJ_DATA        *obj2        = (OBJ_DATA  *) arg2;
        CHAR_DATA       *to;
        CHAR_DATA       *vch         = (CHAR_DATA *) arg2;
        static char *    const  he_she  [ ] = { "it",  "he",  "she" };
        static char *    const  him_her [ ] = { "it",  "him", "her" };
        static char *    const  his_her [ ] = { "its", "his", "her" };
        const  char            *str;
        const  char            *i;
        char            *point;
        char            *pbuff;
        char             buffer  [ MAX_STRING_LENGTH * 2 ];
        char             buf     [ MAX_STRING_LENGTH ];
        char             buf1    [ 2*  MAX_STRING_LENGTH ];
        char             fname   [ MAX_INPUT_LENGTH  ];

        if (!format || format[0] == '\0')
                return;

        to = ch->in_room->people;
        if (type == TO_VICT)
        {
                if (!vch)
                {
                        bug("Act: null vch with TO_VICT.", 0);
                        sprintf(buf1, "Bad act string:  %s", format);
                        bug(buf1, 0);
                        return;
                }
                to = vch->in_room->people;
        }

        for (; to; to = to->next_in_room)
        {
                if ((to->deleted)
                    || (!to->desc && IS_NPC(to))
                    || !IS_AWAKE(to))
                        continue;

                if (type == TO_CHAR    && to != ch)
                        continue;
                if (type == TO_VICT    && (to != vch || to == ch))
                        continue;
                if (type == TO_ROOM    && to == ch)
                        continue;
                if (type == TO_NOTVICT && (to == ch || to == vch))
                        continue;

                point   = buf;
                str     = format;

                while (*str != '\0')
                {
                        if (*str != '$')
                        {
                                *point++ = *str++;
                                continue;
                        }
                        ++str;

                        if (!arg2 && *str >= 'A' && *str <= 'Z'  && *str >= '0' && *str <= '9')
                        {
                                bug("Act: missing arg2 for code %d.", *str);
                                sprintf(buf1, "Bad act string:  %s", format);
                                bug(buf1, 0);
                                i = " <???> ";
                        }
                        else
                        {
                                switch (*str)
                                {
                                    default:  bug("Act: bad code %d.", *str);
                                        sprintf(buf1, "Bad act string:  %s", format);
                                        bug(buf1, 0);
                                        i = " <???> ";
                                        break;

                                    case '1':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = BLUE;
                                        break;

                                    case '2':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = GREEN;
                                        break;

                                    case '3':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = CYAN;
                                        break;

                                    case '4':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = RED;
                                        break;

                                    case '5':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = PURPLE;
                                        break;

                                    case '6':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = YELLOW;
                                        break;

                                    case '7':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = GREY;
                                        break;

                                    case '0':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = BLACK;
                                        break;

                                    case 'B':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI)
                                            || IS_SET(to->act, PLR_VT100))
                                                i = BOLD;
                                        break;

                                    case 'I':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI)
                                            ||  IS_SET(to->act, PLR_VT100))
                                                i = INVERSE;
                                        break;

                                    case 'F':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI)
                                            ||  IS_SET(to->act, PLR_VT100))
                                                i = FLASH;
                                        break;

                                    case 'R':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI)
                                            ||  IS_SET(to->act, PLR_VT100))
                                                i = NTEXT;
                                        break;

                                    case 't':
                                        i = (char *) arg1;
                                        break;

                                    case 'T':
                                        i = (char *) arg2;
                                        break;

                                    case 'n': i = PERS(ch,  to);
                                        break;

                                    case 'N': i = PERS(vch, to);
                                        break;

                                    case 'c':
                                        i = capitalize_initial(PERS(ch, to));
                                        break;

                                    case 'C':
                                        i = capitalize_initial(PERS(vch, to));
                                        break;

                                    case 'e':
                                        i = he_she  [URANGE(0, ch  ->sex, 2)];
                                        break;

                                    case 'E':
                                        i = he_she  [URANGE(0, vch ->sex, 2)];
                                        break;

                                    case 'm':
                                        i = him_her [URANGE(0, ch  ->sex, 2)];
                                        break;

                                    case 'M':
                                        i = him_her [URANGE(0, vch ->sex, 2)];
                                        break;

                                    case 's':
                                        i = his_her [URANGE(0, ch  ->sex, 2)];
                                        break;

                                    case 'S':
                                        i = his_her [URANGE(0, vch ->sex, 2)];
                                        break;

                                    case 'p':
                                        i = can_see_obj(to, obj1)
                                                ? obj1->short_descr
                                                : "something";
                                        break;

                                    case 'P':
                                        i = can_see_obj(to, obj2)
                                                ? obj2->short_descr
                                                : "something";
                                        break;

                                    case 'd':
                                        if (!arg2 || ((char *) arg2)[0] == '\0')
                                                i = "door";
                                        else
                                        {
                                                one_argument((char *) arg2, fname);
                                                i = fname;
                                        }
                                        break;

                                    case 'D':
                                        if (!IS_NPC(ch)
                                            && ch->pcdata->deity_patron > -1
                                            && ch->pcdata->deity_patron < NUMBER_DEITIES)
                                                i = deity_info_table[ch->pcdata->deity_patron].name;
                                        else if (ch->sub_class == SUB_CLASS_INFERNALIST)
                                                i = "demonic entities";
                                        else
                                                i = "God";
                                        break;
                                }
                        }

                        ++str;
                        while ((*point = *i) != '\0')
                                ++point, ++i;
                }

                *point++        = '\n';
                *point++        = '\r';
                *point          = '\0';
                buf[0]         = UPPER(buf[0]);

                pbuff           = buffer;
                colourconv_8bit(pbuff, buf, to);

                if (to->desc && (to->desc->connected == CON_PLAYING))
                        write_to_buffer(to->desc, buffer, 0);
        }

        return;
}
*/

void act_move (const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type)
{
        OBJ_DATA        *obj1        = (OBJ_DATA  *) arg1;
        OBJ_DATA        *obj2        = (OBJ_DATA  *) arg2;
        CHAR_DATA       *to;
        CHAR_DATA       *vch         = (CHAR_DATA *) arg2;
        static char *    const  he_she  [ ] = { "it",  "he",  "she" };
        static char *    const  him_her [ ] = { "it",  "him", "her" };
        static char *    const  his_her [ ] = { "its", "his", "her" };
        const  char            *str;
        const  char            *i;
        char            *point;
        char             buf     [ MAX_STRING_LENGTH ];
        char             buf1    [ MAX_STRING_LENGTH ];
        char             fname   [ MAX_INPUT_LENGTH  ];

        /*
         * Discard null and zero-length messages.
         */
        if (!format || format[0] == '\0')
                return;

        to = ch->in_room->people;

        if (type == TO_VICT)
        {
                if (!vch)
                {
                        bug("Act: null vch with TO_VICT.", 0);
                        sprintf(buf1, "Bad act string:  %s", format);
                        bug(buf1, 0);
                        return;
                }
                to = vch->in_room->people;
        }

        for (; to; to = to->next_in_room)
        {
                if ((to->deleted)
                    || (!to->desc && IS_NPC(to))
                    || !IS_AWAKE(to))
                        continue;

                if (!IS_NPC(to) && IS_AFFECTED(ch, AFF_SNEAK)
                    && !is_affected(to, gsn_detect_sneak))
                                continue;

                if (type == TO_CHAR    && to != ch)
                        continue;
                if (type == TO_VICT    && (to != vch || to == ch))
                        continue;
                if (type == TO_ROOM    && to == ch)
                        continue;
                if (type == TO_NOTVICT && (to == ch || to == vch))
                        continue;

                point   = buf;
                str     = format;
                while (*str != '\0')
                {
                        if (*str != '$')
                        {
                                *point++ = *str++;
                                continue;
                        }
                        ++str;

                        if (!arg2 && *str >= 'A' && *str <= 'Z'  && *str >= '0' && *str <= '9')
                        {
                                bug("Act: missing arg2 for code %d.", *str);
                                sprintf(buf1, "Bad act string:  %s", format);
                                bug(buf1, 0);
                                i = " <???> ";
                        }
                        else
                        {
                                switch (*str)
                                {
                                    default:  bug("Act: bad code %d.", *str);
                                        sprintf(buf1, "Bad act string:  %s", format);
                                        bug(buf1, 0);
                                        i = " <???> ";
                                        break;

                                    case '1':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = BLUE;
                                        break;

                                    case '2':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = GREEN;
                                        break;

                                    case '3':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = CYAN;
                                        break;

                                    case '4':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = RED;
                                        break;

                                    case '5':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = PURPLE;
                                        break;

                                    case '6':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = YELLOW;
                                        break;

                                    case '7':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = GREY;
                                        break;

                                    case '0':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI))
                                                i = BLACK;
                                        break;

                                    case 'B':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI)
                                            ||  IS_SET(to->act, PLR_VT100))
                                                i = BOLD;
                                        break;

                                    case 'I':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI)
                                            ||  IS_SET(to->act, PLR_VT100))
                                                i = INVERSE;
                                        break;

                                    case 'F':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI)
                                            ||  IS_SET(to->act, PLR_VT100))
                                                i = FLASH;
                                        break;

                                    case 'R':
                                        i = "";
                                        if (IS_SET(to->act, PLR_ANSI)
                                            ||  IS_SET(to->act, PLR_VT100))
                                                i = NTEXT;
                                        break;

                                    case 't':
                                        i = (char *) arg1;
                                        break;

                                    case 'T':
                                        i = (char *) arg2;
                                        break;

                                    case 'n':
                                        i = PERS(ch,  to);
                                        break;

                                    case 'N':
                                        i = PERS(vch, to);
                                        break;

                                    case 'e':
                                        i = he_she  [URANGE(0, ch  ->sex, 2)];
                                        break;

                                    case 'E':
                                        i = he_she  [URANGE(0, vch ->sex, 2)];
                                        break;

                                    case 'm':
                                        i = him_her [URANGE(0, ch  ->sex, 2)];
                                        break;

                                    case 'M':
                                        i = him_her [URANGE(0, vch ->sex, 2)];
                                        break;

                                    case 's':
                                        i = his_her [URANGE(0, ch  ->sex, 2)];
                                        break;

                                    case 'S':
                                        i = his_her [URANGE(0, vch ->sex, 2)];
                                        break;

                                    case 'p':
                                        i = can_see_obj(to, obj1)
                                                ? obj1->short_descr
                                                : "something";
                                        break;

                                    case 'P':
                                        i = can_see_obj(to, obj2)
                                                ? obj2->short_descr
                                                : "something";
                                        break;

                                    case 'd':
                                        if (!arg2 || ((char *) arg2)[0] == '\0')
                                                i = "door";
                                        else
                                        {
                                                one_argument((char *) arg2, fname);
                                                i = fname;
                                        }
                                        break;
                                }
                        }

                        ++str;
                        while ((*point = *i) != '\0')
                                ++point, ++i;
                }

                *point++ = '\n';
                *point++ = '\r';
                buf[0]   = UPPER(buf[0]);

                if (to->desc)
                        write_to_buffer(to->desc, buf, point - buf);
        }

        return;
}

void bit_explode (CHAR_DATA *ch, char* buf, long unsigned int n)
{
    char tmp [MAX_STRING_LENGTH] = {0};
    /*char buf2    [ MAX_STRING_LENGTH ];
    char buf3    [ MAX_STRING_LENGTH ];*/
    char *sep = "";
    unsigned long int arr[63] = {0};
    unsigned long int total;
    int i = 0;
    int j;
    int pos;

    /*unsigned long int a[65],k;

    a[0]=1;

    for(k=1;k<65;k++)
    {
        a[k]= 2UL << (k - 1);
        sprintf(buf3, "k=%lu a[k]=%lu\r\n",k, a[k]);
        log_string(buf3);
    }

    sprintf(buf2,"bit_explode was passed: %lu", n);
    log_string(buf2);

    Doesn't accept stupid values
    if (n <= 0 || n > 18446744073709551616)
        return;       */

    /*sprintf(buf2,"%lu %lu %lu %lu %lu", BIT_31, BIT_32, BIT_33, BIT_34, BIT_35);
    log_string(buf2);*/


    strcpy( buf, tmp );

    while (n) {
        pos = 63UL - (uint64_t)__builtin_clzl(n);

        /*sprintf(buf2,"pos is: %d", pos);
        log_string(buf2);*/

        total = pow(2, pos);

        if (total > 0)
        {
            arr[i] = total;
            /*sprintf(buf3,"arr is: %lu", arr);
            log_string(buf3);*/
        }

        i++;
        n ^= 1UL << pos;
    }

    for(j=i; j>=0; j--)
    {
        if (j == i)
            continue;

        if (arr[j] > 0)
        {
            if ((arr[j] == 1) || (arr[j] == 0))
            {
                sep = "";
            }
            sprintf(tmp, "%s%lu", sep, arr[j]);
            strcat( buf, tmp );
            sep = "|";
        }
    }
}

int cat_sprintf(char *dest, const char *fmt, ...)
{
	char buf[MAX_STRING_LENGTH];
	int size;
	va_list args;

	va_start(args, fmt);
	size = vsprintf(buf, fmt, args);
	va_end(args);

	strcat(dest, buf);

	return size;
}

int bgcolour (char type, CHAR_DATA *ch, char *string)
{
        char code[ 20 ];
        char *p = '\0';

        if (IS_NPC(ch))
                return 0;

        switch (type)
        {
            default:
                sprintf(code, B_BLACK);
                break;
            case 'k':
                sprintf(code, B_B_BLACK);
                break;
            case 'K':
                sprintf(code, B_BLACK);
                break;
            case 'b':
                sprintf(code, B_B_BLUE);
                break;
            case 'B':
                sprintf(code, B_BLUE);
                break;
            case 'c':
                sprintf(code, B_CYAN);
                break;
            case 'C':
                sprintf(code, B_B_CYAN);
                break;
            case 'g':
                sprintf(code, B_GREEN);
                break;
            case 'G':
                sprintf(code, B_B_GREEN);
                break;
            case 'm':
                sprintf(code, B_PURPLE);
                break;
            case 'M':
                sprintf(code, B_B_PURPLE);
                break;
            case 'r':
                sprintf(code, B_RED);
                break;
            case 'R':
                sprintf(code, B_B_RED);
                break;
            case 'w':
                sprintf(code, B_GREY);
                break;
            case 'W':
                sprintf(code, B_B_GREY);
                break;
            case 'd':
                sprintf(code, B_D_GREY);
                break;
            case 'D':
                sprintf(code, B_D_B_GREY);
                break;
            case 'y':
                sprintf(code, B_YELLOW);
                break;
            case 'Y':
                sprintf(code, B_B_YELLOW);
                break;
            case '}':
                sprintf(code, "%c", '}');
                break;
        }

        p = code;
        while(*p != '\0')
        {
                *string = *p++;
                *++string = '\0';
        }

        return (strlen(code));
}



void wizPort_handler (int wizPort)
{
        return;
}


/*
 *  Write player and server activity at point of crash to file.
 *  Gezhp 2000
 */
void write_last_command()
{
        int fd;
        static int calls = 0;

        if ((calls++) > 1)
        {
                log_string ("Ack! write_last_command SIGSEGV handler has spewed!");
                abort();
        }

        if (last_command[0] == '\0')
                return;

       /* fd = open ("../log/last_command.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);  */

        if (fd < 0)
                return;

        write (fd, last_command, strlen (last_command));

        if (last_function[0] != '\0')
        {
                write (fd, "Function: ", 10);
                write (fd, last_function, strlen (last_function));
                write (fd, "\n", 1);
        }

        write (fd, "\n", 1);
        close (fd); 
        abort();
}


void install_crash_handler()
{
        last_command[0] = '\0';
        last_function[0] = '\0';

        if (atexit (write_last_command) != 0)
        {
                perror ("install_crash_handler: atexit");
                exit (1);
        }

        signal (SIGSEGV, write_last_command);
}


/*
 *  Send a paragraph less than 80 characters wide and indented
 *  below the first line; Gezhp 2000
 */

void send_paragraph_to_char (char* text, CHAR_DATA* ch, unsigned int indent)
{
        const int       margin = 78;
        char            buf [MAX_STRING_LENGTH];
        int             pos, max, max_line, i, j, k, l;
        bool            indent_line = FALSE;

        if (!text)
                return;

        if (indent > margin / 2)
                indent = margin / 2;

        pos = 0;
        max = strlen (text);

        buf[0] = '\0';

        for (i = 0; i < max; i++)
        {
                k = 0;
                max_line = margin;

                if (indent_line)
                {
                        for (l = 0; l < indent; l++)
                        {
                                strcpy(buf + pos, " ");
                                pos++;
                                max_line--;
                        }
                }
                else
                        indent_line = TRUE;

                for (j = 0; j < max_line; j++)
                {
                        if (i+j >= max)
                        {
                                break;
                        }

                        if (text[i+j] == '\n')
                        {
                                k = j+1;
                                break;
                        }

                        if (text[i+j] == ' ')
                                k = j;

                        if (text[i+j] == '{' || text[i+j] == '}')
                                max_line += 2;
                }

                if (!k)
                        k = max_line;

                for (j = 0; j < k; j++)
                {
                        if (text[i+j] != '\n'
                            && text[i+j] != '\r'
                            && !(j == 0 && text[i+j] == ' '))
                        {
                                buf[pos++] = text[i+j];
                        }
                }

                buf[pos++] = '\n';
                buf[pos++] = '\r';
                i += k;
        }

        buf[pos] = '\0';
        send_to_char (buf, ch);
}

void assert_directory_exists(const char *path)
{
        DIR * dir;
        char buf[MAX_STRING_LENGTH];

        dir = opendir(path);
        if (dir) {
                closedir(dir);
        } else {
                sprintf(buf, "Required directory does not exist or cannot be opened: %s", path);
                bug(buf, MAX_STRING_LENGTH);
                exit(1);
        }
}

void colourconv_8bit (char *buffer, const char *txt , CHAR_DATA *ch)
{
        /*
         * Yes this is terrible, yes it's the best I can do right now,
         * yes it works, yes there are 8(!) levels of nesting, yes you
         * are welcome to try and make it Actually Good.
         *
         * I pray I never have to revisit this unholy hellscape where
         * all good things go to die.  --Owl
         */

        const char *point;
        /* char buf78 [MAX_STRING_LENGTH];*/
        int skip = 0;
        char cur_code[3];
        int cd_count = 2;
        int cv_int;
        char open_token;
        char close_token;

        /* If you change these tokens, you should change the color_table_8bit act_codes as well */
        open_token   = '<';
        close_token  = '>';

        if (ch->desc && txt)
        {
                if (IS_SET(ch->act, PLR_ANSI))
                {
                        for (point = txt ; *point ; point++)
                        {
                                if (*point == '{')
                                {
                                        point++;
                                        skip = colour(*point, ch, buffer);
                                        while (skip-- > 0)
                                                ++buffer;
                                        continue;
                                }

                                if (*point == '}')
                                {
                                        point++;
                                        skip = bgcolour(*point, ch, buffer);
                                        while (skip-- > 0)
                                                ++buffer;
                                        continue;
                                }

                                if (*point == open_token)
                                {
                                        point++;

                                        if (*point == open_token)
                                        {
                                                /* An escaped open_token */

                                                *buffer = *point;
                                                *++buffer = '\0';
                                                continue;
                                        }

                                        if (isdigit(*point))
                                        {
                                                if (cd_count < 0)
                                                        continue;

                                                cur_code[cd_count] = *point;
                                                cd_count--;
                                                point++;

                                                if (isdigit(*point))
                                                {
                                                        if (cd_count < 0)
                                                                continue;

                                                        cur_code[cd_count] = *point;
                                                        cd_count--;
                                                        point++;

                                                        if (isdigit(*point))
                                                        {
                                                                if (cd_count < 0)
                                                                        continue;

                                                                cur_code[cd_count] = *point;
                                                                cd_count--;
                                                                point++;
                                                        }
                                                }
                                        }
                                        else {
                                                /* zero code array, lonely token opener */
                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;
                                                continue;
                                        }

                                        if (*point == close_token)
                                        {
                                                /*
                                                * If we end up here we have the color code
                                                * and everything is sweet.
                                                */
                                                /*sprintf(buf78, "cv_int pre-reverse: %s\r\n", cur_code);
                                                log_string(buf78);*/

                                                reverse_char_array(cur_code, 3);

                                                /*sprintf(buf78, "cur_code char reversed: %s\r\n", cur_code);
                                                log_string(buf78);*/

                                                cv_int = atoi( cur_code );

                                                /*sprintf(buf78, "cv_int to go to colour_8bit(): %d\r\n", cv_int);
                                                log_string(buf78);*/

                                                skip = colour_8bit(cv_int, ch, buffer);

                                                /* zero code array */
                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;

                                                while (skip-- > 0)
                                                        ++buffer;

                                                continue;
                                        }
                                        else {
                                                /* We have an unclosed token error */
                                                /* cv_int = reverse_number( atoi( cur_code ) ); */

                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;

                                                continue;
                                        }
                                }
                                *buffer = *point;
                                *++buffer = '\0';
                        }
                        *buffer = '\0';
                }
                else
                {
                        /*
                         * If the player has ANSI disabled we need to
                         * run a modified version of the above nightmare
                         * code to strip the tokens out of the string.
                         */

                       /* zero code array  to start */
                        cur_code[0] = '\0';
                        cur_code[1] = '\0';
                        cur_code[2] = '\0';
                        cd_count = 2;

                        for (point = txt ; *point ; point++)
                        {

                                if (*point == '{' || *point == '}')
                                {
                                        point++;
                                        continue;
                                }

                                if (*point == open_token)
                                {
                                        point++;

                                        if (*point == open_token)
                                        {
                                                /* An escaped open_token */

                                                *buffer = *point;
                                                *++buffer = '\0';
                                                continue;
                                        }

                                        if (isdigit(*point))
                                        {
                                                if (cd_count < 0)
                                                        continue;

                                                cur_code[cd_count] = *point;
                                                cd_count--;
                                                point++;

                                                if (isdigit(*point))
                                                {
                                                        if (cd_count < 0)
                                                                continue;

                                                        cur_code[cd_count] = *point;
                                                        cd_count--;
                                                        point++;

                                                        if (isdigit(*point))
                                                        {
                                                                if (cd_count < 0)
                                                                        continue;

                                                                cur_code[cd_count] = *point;
                                                                cd_count--;
                                                                point++;
                                                        }
                                                }
                                        }
                                        else {
                                                /* zero code array, lonely token opener */
                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;
                                                continue;
                                        }

                                        if (*point == close_token)
                                        {
                                                /*
                                                * have the color code
                                                */
                                                reverse_char_array(cur_code, 3);
                                                cv_int = atoi( cur_code );
                                                strip_colour_8bit(cv_int, ch, buffer);

                                                /*sprintf(buf78, "cvint = %d skip = %d \n\r", cv_int, skip);
                                                log_string(buf78);*/

                                                /* zero code array */
                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;

                                                continue;
                                        }
                                        else {
                                                /* unclosed token error */
                                                /* cv_int = reverse_number( atoi( cur_code ) ); */

                                                cur_code[0] = '\0';
                                                cur_code[1] = '\0';
                                                cur_code[2] = '\0';
                                                cd_count = 2;

                                                continue;
                                        }
                                }
                                *buffer = *point;
                                *++buffer = '\0';
                        }

                        *buffer = '\0';
                        return;
                }

        }
        return;

}

void strip_colour_8bit (int icode, CHAR_DATA *ch, char *string)
{
        char code[ 20 ];
        char *p = '\0';

        if (IS_NPC(ch))
                return;

        sprintf(code, "%d", icode);

        p = code;

        while(*p != '\0')
        {
                *string = *p++;
                *++string = '\0';
        }
}

int colour_8bit (int icode, CHAR_DATA *ch, char *string)
{
        char code[ 20 ];
        char *p = '\0';
        int i;

        if (IS_NPC(ch))
                return 0;

        for (i = 0; i < MAX_8BIT_COLORS; i++)
        {
                if( icode == color_table_8bit[i].number )
                {
                      sprintf(code, "%s", color_table_8bit[i].code);
                }
        }
        p = code;

        while(*p != '\0')
        {
                *string = *p++;
                *++string = '\0';
        }

        return (strlen(code));
}

int colour (char type, CHAR_DATA *ch, char *string)
{
        char code[ 20 ];
        char *p = '\0';

        if (IS_NPC(ch))
                return 0;

        switch (type)
        {
            default:
                sprintf(code, CLEAR);
                break;
            case 'x':
                sprintf(code, CLEAR);
                break;
            case 'k':
                sprintf(code, BLACK);
                break;
            case 'b':
                sprintf(code, C_BLUE);
                break;
            case 'c':
                sprintf(code, C_CYAN);
                break;
            case 'g':
                sprintf(code, C_GREEN);
                break;
            case 'm':
                sprintf(code, C_MAGENTA);
                break;
            case 'r':
                sprintf(code, C_RED);
                break;
            case 'w':
                sprintf(code, C_WHITE);
                break;
            case 'y':
                sprintf(code, C_YELLOW);
                break;
            case 'B':
                sprintf(code, C_B_BLUE);
                break;
            case 'C':
                sprintf(code, C_B_CYAN);
                break;
            case 'G':
                sprintf(code, C_B_GREEN);
                break;
            case 'M':
                sprintf(code, C_B_MAGENTA);
                break;
            case 'R':
                sprintf(code, C_B_RED);
                break;
            case 'W':
                sprintf(code, C_B_WHITE);
                break;
            case 'Y':
                sprintf(code, C_B_YELLOW);
                break;
            case 'D':
                sprintf(code, C_D_GREY);
                break;
            case 'd':
                sprintf(code, D_GREY);
                break;
            case '*':
                 sprintf(code, "%c", 007);
                 break;
            case '/':
                sprintf(code, "%c", 012);
                break;
            case '{':
                sprintf(code, "%c", '{');
                break;
        }

        p = code;
        while(*p != '\0')
        {
                *string = *p++;
                *++string = '\0';
        }

        return (strlen(code));
}

void colourconv (char *buffer, const char *txt , CHAR_DATA *ch)
{
        const char *point;
        int skip = 0;

        if (ch->desc && txt)
        {
                if (IS_SET(ch->act, PLR_ANSI))
                {
                        for (point = txt ; *point ; point++)
                        {
                                if (*point == '{')
                                {
                                        point++;
                                        skip = colour(*point, ch, buffer);
                                        while (skip-- > 0)
                                                ++buffer;
                                        continue;
                                }

                                if (*point == '}')
                                {
                                        point++;
                                        skip = bgcolour(*point, ch, buffer);
                                        while (skip-- > 0)
                                                ++buffer;
                                        continue;
                                }

                                *buffer = *point;
                                *++buffer = '\0';
                        }
                        *buffer = '\0';
                }
                else
                {
                        for (point = txt ; *point ; point++)
                        {
                                if (*point == '{' || *point == '}')
                                {
                                        point++;
                                        continue;
                                }
                                *buffer = *point;
                                *++buffer = '\0';
                        }
                        *buffer = '\0';
                }
        }
        return;
}

int reverse_number ( int number )
{
        int reversed;
        reversed = 0;

        while (number != 0)
        {
                reversed = reversed * 10;
                reversed = reversed + number % 10;
                number   = number / 10;
        }
        return reversed;
}


void reverse_char_array(char arr[], int n)
{
    /* Reverse elements of an array in-place where n = num of elements */
    int low;
    int high;
    int temp;

    for (low = 0, high = n - 1; low < high; low++, high--)
    {
        temp = arr[low];
        arr[low] = arr[high];
        arr[high] = temp;
    }
}

int digits_in_int (int number )
{
        int count = 0;

        do {
                number /= 10;
                ++count;
        }
        while ( number != 0 );

        return count;
}

/* GCMP */
void ch_printf(CHAR_DATA *ch, const char *fmt, ...)
{
	char buf[MAX_STRING_LENGTH];
	va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	send_to_char(buf, ch);
}


void log_printf (char * fmt, ...)
{
	char buf [MAX_STRING_LENGTH];
	va_list args;

	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end (args);

	log_string (buf);
}

void log_build_printf (int vnum, char * fmt, ...)
{
	char buf [MAX_STRING_LENGTH];
	va_list args;

	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end (args);

	/* log_build_string (vnum, buf); removed Brutus */
}

void pop_call(void)
{
	if (call_index != 0)
	{
		call_index--;
	}
	else
	{
		log_printf("pop_call: index is zero: %s", call_stack[0]);
	}
}


void push_call_format(char *f, ...)
{
        va_list ap;

        va_start(ap, f);

        if (call_index >= MAX_STACK_CALL)
        {
                call_index = MAX_STACK_CALL / 2;
                log_printf("push_call: max stack size reached");
                dump_stack();
                call_index = 0;
        }

/*      strcpy(call_stack[call_index], f); */

        vsprintf(call_stack[call_index], f, ap);

        call_index++;

        va_end(ap);
}

void push_call(char *f, ...)
{
	va_list ap;

	va_start(ap, f);

	if (call_index >= MAX_STACK_CALL)
	{
		call_index = MAX_STACK_CALL / 2;
		log_printf("push_call: max stack size reached");
		dump_stack();
		call_index = 0;
	}

/*	strcpy(call_stack[call_index], f); */

	vsprintf(call_stack[call_index], f, ap);

	call_index++;
}

void dump_stack(void)
{
	int i;

	log_printf("Stack trace (index = %d)", call_index);

	for (i = 0 ; i < call_index ; i++)
	{
		log_printf("call_stack[%03d] = %s", i, call_stack[i]);
	}
	log_printf("End of stack");
}

void force_help(DESCRIPTOR_DATA *d, char *argument)
{
	AREA_DATA *pArea;
	HELP_DATA *pHelp;

        printf(log_buf, "Sent a force_help.");
        log_string(log_buf);
	push_call("force_help(%p,%p)",d,argument);

	for (pArea = mud->f_area ; pArea ; pArea = pArea->next)
	{
		for (pHelp = pArea->first_help ; pHelp ; pHelp = pHelp->next)
		{
			if (is_name(argument, pHelp->keyword))
			{
				write_to_buffer(d, ansi_translate(pHelp->text), 1000000);
				pop_call();
				return;
			}
		}
	}
	pop_call();
	return;
}

/*
	Scandum - 03-05-2002
*/

void display_empty_screen(DESCRIPTOR_DATA *d)
{
	push_call("display_empty_screen(%p)",d);

	if (d->character == NULL || !HAS_BIT(d->character->pcdata->vt100_flags, VT100_INTERFACE))
	{
		write_to_buffer(d, "\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r", 1000000);
		write_to_buffer(d, "\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r", 1000000);
		write_to_buffer(d, "\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r", 1000000);
		write_to_buffer(d, "\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r", 1000000);
	}
	else
	{
		write_to_buffer(d, "\033[2J", 1000000);
	}
	pop_call();
	return;
}

bool check_reconnecting(DESCRIPTOR_DATA *d, CHAR_DATA *ch)
{
	DESCRIPTOR_DATA *desc;

	push_call("check_reconnecting(%p,%p)",d,ch);

	if (d->lookup == FALSE)
	{
		pop_call();
		return FALSE;
	}

	for (desc = mud->f_desc ; desc ; desc = desc->next)
	{
		if (desc->lookup == FALSE && CH(desc) == ch)
		{
			desc->original = desc->character = NULL;
			SET_BIT(desc->mth->comm_flags, COMM_FLAG_DISCONNECT);
			break;
		}
	}

	ch->desc     = d;
	d->character = ch;
	d->original  = NULL;

	pop_call();
	return TRUE;
}

void remove_bad_desc_name(char *name)
{
	DESCRIPTOR_DATA *d;

	push_call("remove_bad_desc_name(%p)",name);

	for (d = mud->f_desc ; d ; d = d->next)
	{
		if (d->connected > CON_PLAYING && d->character)
		{
			if (!strcasecmp(d->character->name, name))
			{
				/* log_god_printf("failed connect: %s@%s", d->character->name, d->host); */
				 sprintf(log_buf, "failed connect: %s@%s", d->character->name, d->host);
                                log_string(log_buf);
				if (get_char_pvnum(d->character->pcdata->pvnum) == NULL)
				{
					d->character->desc = NULL;
					extract_char(d->character, TRUE);
				}
				d->character = NULL;
				SET_BIT(d->mth->comm_flags, COMM_FLAG_DISCONNECT);
			}
		}
	}
	pop_call();
	return;
}

void sub_player(CHAR_DATA *ch)
{
	PLAYER_GAME *fpl;

	push_call("sub_player(%p)",ch);

	for (fpl = mud->f_player ; fpl ; fpl = fpl->next)
	{
		if (fpl->ch == ch)
		{
			UNLINK(fpl, mud->f_player, mud->l_player, next, prev);
			FREEMEM(fpl);
			mud->total_plr--;
			break;
		}
	}

	if (get_char_pvnum(ch->pcdata->pvnum) != ch)
	{
		pop_call();
		return;
	}

	pvnum_index[ch->pcdata->pvnum]->ch = NULL;

	pop_call();
	return;
}

char *get_gradient(bool gradient, int current, int max)
{
	int percent;

	push_call("get_gradient(%p,%p,%p)",gradient,current,max);

	if (gradient == FALSE)
	{
		pop_call();
		return "";
	}

	percent = 100 * UMAX(0, current) / UMAX(1, max);

	switch (percent / 25)
	{
		case 0:
			pop_call();
			return "{118}";
		case 1:
		case 2:
			pop_call();
			return "{138}";
	}
	pop_call();
	return "{128}";
}

char *prompt_return(CHAR_DATA *ch, char *layout)
{
	AFFECT_DATA *paf;
	EXIT_DATA *pexit;
	static char prompt_buffer[MAX_INPUT_LENGTH];
	char tbuf[MAX_INPUT_LENGTH];
	char *pti, *pto;
	bool last_was_str, gradient;
	int door;
	struct tm clk;

	push_call("prompt_return(%p,%p)",ch,layout);

	if (!IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL/* && ch->pcdata->editmode != MODE_NONE*/ && ch->pcdata->subprompt && ch->pcdata->subprompt[0] != '\0')
	{
		sprintf (prompt_buffer, "{138}<{158}%s{138}> {078}", ch->pcdata->subprompt);
	}
	else if (layout == NULL || *layout == '\0')
	{
		sprintf(prompt_buffer,  "%s<%dhp %dm %dmv %dxp> ", get_color_string(ch, COLOR_PROMPT, VT102_DIM), ch->hit, ch->mana, ch->move, exp_level(ch->class, ch->level) - (IS_NPC(ch) ? 0 : ch->pcdata->exp));
	}
	else
	{
		last_was_str = FALSE;
		gradient     = FALSE;
		pti  = layout;
		strcpy(prompt_buffer, get_color_string(ch, COLOR_PROMPT, VT102_DIM));
		pto  = prompt_buffer;
		pto += strlen(prompt_buffer);

		while(*pti != '\0')
		{
			if (last_was_str)
			{
				last_was_str = FALSE;

				switch (*pti++)
				{
					case 'h':
						sprintf(tbuf, "%s%d", get_gradient(gradient, ch->hit, ch->max_hit), ch->hit);
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 'H':
						sprintf(tbuf, "%s%d", get_gradient(gradient, ch->hit, ch->max_hit), ch->max_hit);
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;

					case 'v':
						sprintf(tbuf, "%s%d", get_gradient(gradient, ch->move, ch->max_move), ch->move);
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 'V':
						sprintf(tbuf, "%s%d", get_gradient(gradient, ch->move, ch->max_move), ch->max_move);
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 'm':
						sprintf(tbuf, "%s%d", get_gradient(gradient, ch->mana, ch->max_mana), ch->mana);
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 'M':
						sprintf(tbuf, "%s%d", get_gradient(gradient, ch->mana, ch->max_mana), ch->max_mana);
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 'x':
						sprintf(tbuf, "%s%d", get_gradient(gradient, exp_level(ch->class, ch->level) - (IS_NPC(ch) ? 0 : ch->pcdata->exp), exp_level(ch->class, ch->level) - exp_level(ch->class, ch->level-1)), IS_NPC(ch) ? 0 : ch->pcdata->exp);
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 'X':
						sprintf(tbuf, "%s%d", get_gradient(gradient, exp_level(ch->class, ch->level) - (IS_NPC(ch) ? 0 : ch->pcdata->exp), exp_level(ch->class, ch->level) - exp_level(ch->class, ch->level-1)), exp_level(ch->class, ch->level) - (IS_NPC(ch) ? 0 : ch->pcdata->exp));
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 'l':
						sprintf(tbuf, "%s%d", get_gradient(gradient, ch->hit, ch->max_hit), 100 * ch->hit / UMAX(1, ch->max_hit));
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 'L':
						if (who_fighting(ch))
						{
							sprintf(tbuf, "%s%d", get_gradient(gradient, ch->fighting->hit, ch->fighting->max_hit), 100 * ch->fighting->hit / UMAX(1, ch->fighting->max_hit));
						}
						else
						{
							sprintf(tbuf, "--");
						}
						strcpy(pto, tbuf);
						pto += strlen (tbuf);
						break;
					case 'g':
						sprintf(tbuf, "%s%d", get_gradient(gradient, ch->gold, ch->level * 1000000), ch->gold);
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 'a':
						sprintf(tbuf, "%s%d", get_gradient(gradient, ch->alignment + 1000, 2000), ch->alignment);
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 's':
						switch (ch->speed)
						{
							case 0: *pto = 'W'; break;
							case 1: *pto = 'N'; break;
							case 2: *pto = 'J'; break;
							case 3: *pto = 'R'; break;
							case 4: *pto = 'H'; break;
							case 5: *pto = 'B'; break;
						}
						pto++;
						break;

					/*case 'S':
						strcpy(tbuf, language_table[UNSHIFT(CH(ch->desc)->pcdata->speak)].name);
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					*/	
					case 'f':
						for (paf = ch->first_affect ; paf ; paf = paf->next)
						{
							*pto = skill_table[paf->type].name[0] - 32;	pto++;
							*pto = skill_table[paf->type].name[1];		pto++;
						}
						break;
					case 'e':
						if (can_see_in_room(ch, ch->in_room))
						{
							for (door = 0 ; door < 6 ; door++)
							{
								if ((pexit = get_exit(ch->in_room->vnum, door)) != NULL
								/*&&   !IS_SET(pexit->flags, EX_CLOSED)
								&&  (!IS_SET(ch->in_room->room_flags, ROOM_SMOKE)	|| can_see_smoke(ch))
								&&  (!IS_SET(pexit->flags, EX_HIDDEN) || can_see_hidden(ch))
								&&   can_use_exit(ch, pexit)*/)
								{
									switch (door)
									{
										case 0: *pto = 'N'; pto++; break;
										case 1: *pto = 'E'; pto++; break;
										case 2: *pto = 'S'; pto++; break;
										case 3: *pto = 'W'; pto++; break;
										case 4: *pto = 'U'; pto++; break;
										case 5: *pto = 'D'; pto++; break;
									}
								}
							}
						}
						break;
					case 'w':
						ch->wait ? (*pto = '*') : (*pto = ' ');
						pto++;
						break;
					case 't':
						sprintf(tbuf, "%d", mud->time_info->hour);
                                                /*sprintf(tbuf, "%s", tocivilian(mud->time_info->hour));*/
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break;
					case 'T':
						clk = *localtime (&mud->current_time);
						if (CH(ch->desc)->pcdata->clock > 99)
						{
							sprintf(tbuf, "%2d:%02d", (clk.tm_hour + (CH(ch->desc)->pcdata->clock % 100)) % 24, clk.tm_min);
						}
						else
						{
							sprintf(tbuf, "%2d:%02d %s", clk.tm_hour % 12 != 0 ? clk.tm_hour % 12 : 12, clk.tm_min, clk.tm_hour >= 12 ? "pm" : "am");
						}
					/*	if (IS_SET(CH(ch->desc)->act, PLR_WIZTIME))
						{
							sprintf(tbuf, "%02d:%02d", clk.tm_sec, (int) (get_game_usec() % 1000000 / 10000));
						}
						strcpy(pto, tbuf);
						pto += strlen(tbuf);
						break; */
					case '$':
						*pto = '$';
						pto++;
						break;
					case '/':
						strcpy(pto, "\n\r");
						pto += 2;
						break;
					case 'c':
						gradient = TRUE;
						continue;
						break;
					default:
						*pto = '$';
						pto ++;
						*pto = *pti;
						pto ++;
						break;
				}
				gradient = FALSE;
			}
			else
			{
				if (*pti != '$')
				{
					*pto = *pti;
					pto++;
				}
				else
				{
					last_was_str = TRUE;
				}
				pti++;
			}
		}
		*pto = '\0';
	}

	pop_call();
	return (ansi_translate_text(ch, prompt_buffer));
}

long long get_game_usec( void )
{
	struct timeval last_time;

	push_call("get_game_usec()");

	gettimeofday(&last_time, NULL);

	pop_call();
	return((long long) last_time.tv_usec + 1000000LL * (long long) last_time.tv_sec);
}

void add_player(CHAR_DATA *ch)
{
	PLAYER_GAME *gpl, *fpl;

	push_call("add_player(%p)",ch);

	if (get_char_pvnum(ch->pcdata->pvnum) != NULL)
	{
		log_printf("add_player: already listed as playing");
		dump_stack();
	}
	else
	{
		pvnum_index[ch->pcdata->pvnum]->ch = ch;
		REMOVE_BIT(pvnum_index[ch->pcdata->pvnum]->flags, PVNUM_DELETED);
	}

	ALLOCMEM(gpl, PLAYER_GAME, 1);
	gpl->ch = ch;

	mud->total_plr++;

	for (fpl = mud->f_player ; fpl ; fpl = fpl->next)
	{
		if (ch->level > fpl->ch->level)
		{
			continue;
		}
		if (ch->level < fpl->ch->level || ch->pcdata->played < fpl->ch->pcdata->played)
		{
			INSERT_LEFT(gpl, fpl, mud->f_player, next, prev);
			break;
		}
	}
	if (fpl == NULL)
	{
		LINK(gpl, mud->f_player, mud->l_player, next, prev);
	}

	pop_call();
	return;
}

void display_class_selections(DESCRIPTOR_DATA *d)
{
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *ch;
	int iClass;

	push_call("display_class_selection(%p)",d);

	ch = d->character;

	strcpy(buf, "{038}You may choose from the following classes, or type help [class] to learn more:\n\r\n\r");
	write_to_buffer(d, (char *)ansi_translate_text(ch, buf), 1000000);

	strcpy(buf, "{168}     ");

	for (iClass = 0 ; iClass < MAX_CLASS ; iClass++)
	{
		if (strlen(buf) == 70)
		{
			strcat(buf, "\n\r{168}     ");
		}
		cat_sprintf(buf, "%-15s", class_table[iClass].show_name);
	}
	strcat(buf, "\n\r\n\r{038}Please choose a class: {118}");
	write_to_buffer(d, (char *) ansi_translate_text(ch, buf), 1000000);

	pop_call();
	return;
}

void display_class_details(DESCRIPTOR_DATA *d, int class)
{
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *ch;
	int iRace;

	push_call("display_class_details(%p,%p)",d,class);

	ch = d->character;

	display_empty_screen(d);

	strcpy(buf, class_table[class].show_name);
	strcat(buf, "intro");
	do_help(ch, buf);

	write_to_buffer(d, (char *)ansi_translate_text(ch, buf), 1000000);

	sprintf(buf, "{168}     ");

	strcat(buf, "\n\r\n\r");

	write_to_buffer(d, (char *)ansi_translate_text(ch, buf), 1000000);

	display_class_selections(d);

	pop_call();
	return;
}

void display_race_selections(DESCRIPTOR_DATA *d)
{
	char buf[MAX_INPUT_LENGTH];
	sh_int iRace;
	CHAR_DATA *ch;

	push_call("display_race_selections(%p)", d);

	ch = d->character;

	strcpy(buf, "\n\r{038}You may choose from the following races, or type help [race] to learn more:\n\r\n\r");
	write_to_buffer(d, (char *)ansi_translate_text(ch, buf), 1000000);

	strcpy(buf, "{168}     ");

	for (iRace = 0 ; iRace < MAX_RACE ; iRace++)
	{

		if (strlen(buf) == 70)
		{
			strcat(buf, "\n\r{168}     ");
		}
		cat_sprintf(buf, "%-15s", race_table[iRace].race_name);
	}
	write_to_buffer(d, (char *)ansi_translate_text(ch, buf), 1000000);

	strcpy(buf, "\n\r\n\r{038}Please choose a race: {118}");
	write_to_buffer(d, (char *)ansi_translate_text(ch, buf), 1000000);

	pop_call();
	return;
}

/* end GCMP */

/* EOF comm.c */
