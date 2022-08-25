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
#include "protocol.h"

/*
 * Malloc debugging stuff.
 */
#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern  int     malloc_debug    args((int));
extern  int     malloc_verify   args((void));
#endif


/*
 * Signal handling.
 */
#include <signal.h>


/*
 * Socket and TCP/IP stuff.
 */
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>

/*
 * identd
 */
#if !defined(STDOUT_FILENO)
#define STDOUT_FILENO 1
#endif


const   char    echo_off_str    [] = { IAC, WILL, TELOPT_ECHO, '\0' };
const   char    echo_on_str     [] = { IAC, WONT, TELOPT_ECHO, '\0' };
const   char    go_ahead_str    [] = { IAC, GA, '\0' };


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

void    game_loop_unix          args((int control, int wizPort));
int     init_socket             args((int port));
void    new_descriptor          args((int control));
void    wizPort_handler args((int control));
bool    read_from_descriptor    args((DESCRIPTOR_DATA *d));
bool    write_to_descriptor     args((int desc, char *txt, int length));

int connection_count = 0;
DESCRIPTOR_DATA *initiative_list [MAX_CONNECTIONS];


/*
 * Other local functions (OS-independent).
 */
bool    check_parse_name        args((char *name));
bool    check_reconnect         args((DESCRIPTOR_DATA *d, char *name, bool fConn));
bool    check_playing           args((DESCRIPTOR_DATA *d, char *name));
int     main                    args((int argc, char **argv));
void    nanny                   args((DESCRIPTOR_DATA *d, char *argument));
bool    process_output          args((DESCRIPTOR_DATA *d, bool fPrompt));
void    read_from_buffer        args((DESCRIPTOR_DATA *d));
void    stop_idling             args((CHAR_DATA *ch));
void    bust_a_prompt           args((DESCRIPTOR_DATA *d));
void    assert_directory_exists args((const char *path));


int main(int argc, char **argv)
{
        struct timeval now_time;
        int port;
        int control, wizPort = 0;

        /*
         * Memory debugging if needed.
         */
#if defined(MALLOC_DEBUG)
        malloc_debug(2);
#endif

        /*
         * Init time.
         */
        gettimeofday(&now_time, NULL);
        current_time = (time_t) now_time.tv_sec;
        boot_time = current_time;
        strcpy(str_boot_time, ctime(&boot_time));

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
        port = 8888;
        if (argc > 1)
        {
                if (!is_number(argv[1]))
                {
                        fprintf(stderr, "Usage: %s [port #]\n", argv[0]);
                        exit(1);
                }
                else if ((port = atoi(argv[1])) <= 1024)
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
        mudport = port;
        control = init_socket(port);
        boot_db();
        sprintf(log_buf, "DD is ready to rock on port %d.", port);
        log_string(log_buf);
        game_loop_unix(control, wizPort);
        close(control);

        /*
         * That's all, folks.
         */
        log_string("Normal termination of game.");
        last_command[0] = '\0';
        exit(0);
        return 0;
}


int init_socket(int port)
{
        static struct sockaddr_in sa_zero;
        struct sockaddr_in sa;
        int x  = 1;
        int fd;

        last_command[0] = '\0';

        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
                perror("Init_socket: socket");
                exit(1);
        }

        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                       (char *) &x, sizeof(x)) < 0)
        {
                perror("Init_socket: SO_REUSEADDR");
                close(fd);
                exit(1);
        }

#if defined(SO_DONTLINGER) && !defined(SYSV)
        {
                struct  linger  ld;

                ld.l_onoff  = 1;
                ld.l_linger = 1000;

                if (setsockopt(fd, SOL_SOCKET, SO_DONTLINGER,
                               (char *) &ld, sizeof(ld)) < 0)
                {
                        perror("Init_socket: SO_DONTLINGER");
                        close(fd);
                        exit(1);
                }
        }
#endif

        sa                  = sa_zero;
        sa.sin_family       = AF_INET;
        sa.sin_port         = htons(port);

        if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0)
        {
                perror("Init_socket: bind");
                close(fd);
                exit(1);
        }

        if (listen(fd, 3) < 0)
        {
                perror("Init_socket: listen");
                close(fd);
                exit(1);
        }

        /* system("rm SHUTDOWN.TXT"); */

        {
                char buf[128];
                char *when;

                when = ctime(&current_time);
                when[strlen(when)-1] = '\0';
                sprintf(buf, "Log starts at %s", when);
                log_string(buf);

                sprintf(buf, "Init_socket on port %d (returning fd: %d).", port, fd);
                log_string(buf);
        }

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



void game_loop_unix(int control, int wizPort)
{
        static struct timeval null_time;
        struct timeval last_time;

        signal(SIGPIPE, SIG_IGN);
        gettimeofday(&last_time, NULL);
        current_time = (time_t) last_time.tv_sec;

        install_crash_handler();

        /* Main loop */
        while (!merc_down)
        {
                DESCRIPTOR_DATA *d;
                fd_set           in_set;
                fd_set           out_set;
                fd_set           exc_set;
                int              maxdesc;

                /*
                 fd_set           wizPort_in_set;
                 fd_set           wizPort_out_set;
                 fd_set           wizPort_exc_set;
                 int              wizPort_maxdesc;
                 */

#if defined(MALLOC_DEBUG)
                if (malloc_verify() != 1)
                        abort();
#endif

                /*
                 * Poll all active descriptors.
                 */
                FD_ZERO(&in_set);
                FD_ZERO(&out_set);
                FD_ZERO(&exc_set);
                FD_SET(control, &in_set);

                maxdesc = control;

                /*
                 FD_ZERO(&wizPort_in_set);
                 FD_ZERO(&wizPort_out_set);
                 FD_ZERO(&wizPort_exc_set);
                 FD_SET(wizPort, &wizPort_in_set);
                 wizPort_maxdesc = wizPort;
                 */

                for (d = descriptor_list; d; d = d->next)
                {
                        maxdesc = UMAX(maxdesc, d->descriptor);
                        FD_SET(d->descriptor, &in_set);
                        FD_SET(d->descriptor, &out_set);
                        FD_SET(d->descriptor, &exc_set);
                        if (d->ifd!=-1 && d->ipid!=-1)
                        {
                                maxdesc = UMAX(maxdesc, d->ifd);
                                FD_SET(d->ifd, &in_set);
                        }
                }

                if (select(maxdesc+1, &in_set, &out_set, &exc_set, &null_time) < 0
                    /*|| select(maxdesc+2, &wizPort_in_set, &out_set,
                    &exc_set, &null_time) < 0*/)
                {
                        perror("Game_loop: select: poll");
                        exit(1);
                }

                /*
                 * New connection?
                 */
                if (FD_ISSET(control, &in_set))
                        new_descriptor(control);

                /*
                if (FD_ISSET(wizPort, &wizPort_in_set))
                        wizPort_handler(wizPort);
                 */

                /*
                 * Kick out the freaky folks.
                 */
                for (d = descriptor_list; d; d = d_next)
                {
                        d_next = d->next;
                        if (FD_ISSET(d->descriptor, &exc_set))
                        {
                                FD_CLR(d->descriptor, &in_set);
                                FD_CLR(d->descriptor, &out_set);
                                if (d->character)
                                        save_char_obj(d->character);
                                d->outtop = 0;
                                close_socket(d);
                        }
                }

                /*
                 * Process input.
                 */
                for (d = descriptor_list; d; d = d_next)
                {
                        d_next  = d->next;
                        d->fcommand     = FALSE;

                        if (FD_ISSET(d->descriptor, &in_set))
                        {
                                if (d->character)
                                        d->character->timer = 0;
                                if (!read_from_descriptor(d))
                                {
                                        FD_CLR(d->descriptor, &out_set);
                                        if (d->character)
                                                save_char_obj(d->character);
                                        d->outtop = 0;
                                        close_socket(d);
                                        continue;
                                }
                        }

                        /* check for input from the ident */
                        if ((d->connected==CON_PLAYING || CH(d) != NULL)
                            && d->ifd != -1 && FD_ISSET(d->ifd, &in_set))
                                process_ident(d);

                        if (d->character
                            && d->character->wait > 0
                            && !IS_TRUE_IMMORTAL(d->character))
                        {
                                --d->character->wait;
                                continue;
                        }

                        read_from_buffer(d);
                        if (d->incomm[0] != '\0')
                        {
                                d->fcommand     = TRUE;
                                 if ( d->pProtocol != NULL )
                                        d->pProtocol->WriteOOB = 0;
                                stop_idling(d->character);

                                if (d->connected == CON_PLAYING)
                                        if (d->showstr_point)
                                                show_string(d, d->incomm);
                                else
                                        interpret(d->character, d->incomm);
                                else
                                        nanny(d, d->incomm);

                                d->incomm[0]    = '\0';
                        }
                }

                /*
                 * Autonomous game motion.
                 */
                update_handler();

                /*
                 * Output.
                 */
                for (d = descriptor_list; d; d = d_next)
                {
                        d_next = d->next;

                        if ((d->fcommand || d->outtop > 0)
                            && FD_ISSET(d->descriptor, &out_set))
                        {
                                if (!process_output(d, TRUE))
                                {
                                        if (d->character)
                                                save_char_obj(d->character);
                                        d->outtop       = 0;
                                        close_socket(d);
                                }
                        }
                }

                /*
                 * Synchronize to a clock.
                 * Sleep(last_time + 1/PULSE_PER_SECOND - now).
                 * Careful here of signed versus unsigned arithmetic.
                 */
                {
                        struct timeval now_time;
                        long secDelta;
                        long usecDelta;

                        gettimeofday(&now_time, NULL);

                        usecDelta = ((int) last_time.tv_usec)
                                - ((int)  now_time.tv_usec)
                                + 1000000 / PULSE_PER_SECOND;
                        secDelta = ((int) last_time.tv_sec)
                                - ((int)  now_time.tv_sec);

                        while (usecDelta < 0)
                        {
                                usecDelta += 1000000;
                                secDelta  -= 1;
                        }

                        while (usecDelta >= 1000000)
                        {
                                usecDelta -= 1000000;
                                secDelta  += 1;
                        }

                        if (secDelta > 0 || (secDelta == 0 && usecDelta > 0))
                        {
                                struct timeval stall_time;

                                stall_time.tv_usec = usecDelta;
                                stall_time.tv_sec  = secDelta;

                                if (select(0, NULL, NULL, NULL, &stall_time) < 0)
                                {
                                        perror("Game_loop: select: stall");
                                        exit(1);
                                }
                        }
                }

                gettimeofday(&last_time, NULL);
                current_time = (time_t) last_time.tv_sec;
        }

        return;
}


void new_descriptor (int control)
{
        BAN_DATA               *pban;
        static DESCRIPTOR_DATA  d_zero;
        DESCRIPTOR_DATA        *dnew;
        struct sockaddr_in      sock;
        struct hostent         *from;
        char                    buf [ MAX_STRING_LENGTH ];
        int                     desc;
        socklen_t               size;
        int                     addr;


        size = sizeof(sock);
        getsockname(control, (struct sockaddr *) &sock, &size);

        if ((desc = accept(control, (struct sockaddr *) &sock, &size)) < 0)
        {
                perror("New_descriptor: accept");
                return;
        }

#if !defined(FNDELAY)
# if defined(__hpux)
#  define FNDELAY O_NONBLOCK
# else
#  define FNDELAY O_NDELAY
# endif
#endif

        if (fcntl(desc, F_SETFL, FNDELAY) == -1)
        {
                perror("New_descriptor: fcntl: FNDELAY");
                return;
        }

        /*
         * Cons a new descriptor.
         */
        if (!descriptor_free)
        {
                dnew            = alloc_perm(sizeof(*dnew));
        }
        else
        {
                dnew            = descriptor_free;
                descriptor_free = descriptor_free->next;
        }

        *dnew           = d_zero;
        dnew->descriptor        = desc;
        dnew->character     = NULL;
        dnew->connected = CON_GET_NAME;
        dnew->showstr_head  = str_dup("");
        dnew->showstr_point = 0;
        dnew->outsize   = 2000;
        dnew->outbuf    = alloc_mem(dnew->outsize);
        dnew->pProtocol     = ProtocolCreate(); /* <--- GCMP */

        size = sizeof(sock);

        /*
         * Would be nice to use inet_ntoa here but it takes a struct arg,
         * which ain't very compatible between gcc and system libraries.
         */

        addr = ntohl (sock.sin_addr.s_addr);
        sprintf(buf, "%d.%d.%d.%d",
                (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
                (addr >>  8) & 0xFF, (addr) & 0xFF);
        sprintf(log_buf, "New socket from: %s", buf);
        log_string(log_buf);

        /* from = gethostbyaddr((char *) &sock.sin_addr,
                sizeof(sock.sin_addr), AF_INET); */
        from = NULL;

        dnew->host = str_dup(from ? from->h_name : buf);
        if (from) sprintf (log_buf, "New socket from: %s", from->h_name);
        server_message (log_buf);

        /*
         *  Can we accept any more connections?
         */
        if (connection_count >= MAX_CONNECTIONS)
        {
                write_to_descriptor(desc, "Sorry, too many characters are playing at the moment.\n\r", 0);
                close(desc);
                free_string(dnew->host);
                free_mem(dnew->outbuf, dnew->outsize);
                dnew->next = descriptor_free;
                descriptor_free = dnew;
                return;
        }

        /*
         * Swiftest: I added the following to ban sites.  I don't
         * endorse banning of sites, but Copper has few descriptors now
         * and some people from certain sites keep abusing access by
         * using automated 'autodialers' and leaving connections hanging.
         *
         * Furey: added suffix check by request of Nickel of HiddenWorlds.
         * Shade: changed str_suffix to str_prefix
         */
        for (pban = ban_list; pban; pban = pban->next)
        {
                if (!str_prefix(pban->name, dnew->host))
                {
                        write_to_descriptor(desc, "Your site has been banned from DD.\n\r", 0);
                        close(desc);
                        free_string(dnew->host);
                        free_mem(dnew->outbuf, dnew->outsize);
                        dnew->next = descriptor_free;
                        descriptor_free = dnew;
                        return;
                }
        }

        /*
         * Init descriptor data.
         */
        dnew->next = descriptor_list;
        descriptor_list = dnew;
        ProtocolNegotiate(dnew); /* <--- GCMP */
        connection_count++;
        /*
         * Send the greeting.
         */
        {
                extern char * help_greeting;

                if (help_greeting[0] == '.')
                        write_to_buffer(dnew, help_greeting+1, 0);
                else
                        write_to_buffer(dnew, help_greeting  , 0);
        }

        return;
}


void close_socket (DESCRIPTOR_DATA *dclose)
{
        CHAR_DATA *ch;
        DESCRIPTOR_DATA *d;

        if (dclose->outtop > 0)
                process_output(dclose, FALSE);

        if (dclose->snoop_by)
        {
                write_to_buffer(dclose->snoop_by,
                                "Your victim has left the game.\n\r", 0);
        }

        for (d = descriptor_list; d; d = d->next)
        {
                if (d->snoop_by == dclose)
                        d->snoop_by = NULL;
        }

        if ((ch = dclose->character))
        {
                sprintf(log_buf, "Closing link to %s.", ch->name);
                log_string(log_buf);

                if ((dclose->connected == CON_PLAYING)
                    || ((dclose->connected >= CON_NOTE_TO)
                        && (dclose->connected <= CON_NOTE_FINISH)))
                {
                        act("$n has lost $s link.", ch, NULL, NULL, TO_ROOM);
                        ch->desc = NULL;
                }
                else
                {
                        free_char(dclose->character);
                }
        }

        if (d_next == dclose)
                d_next = d_next->next;

        if (dclose == descriptor_list)
        {
                descriptor_list = descriptor_list->next;
        }
        else
        {
                DESCRIPTOR_DATA *d;

                for (d = descriptor_list; d && d->next != dclose; d = d->next)
                        ;
                if (d)
                        d->next = dclose->next;
                else
                        bug("Close_socket: dclose not found.", 0);
        }

        close(dclose->descriptor);

        free_string(dclose->host);

        /* RT socket leak fix */
        free_mem(dclose->outbuf, dclose->outsize);
         ProtocolDestroy( dclose->pProtocol ); /* <--- GCMP */
        dclose->next    = descriptor_free;
        descriptor_free = dclose;
        connection_count--;
}


bool read_from_descriptor (DESCRIPTOR_DATA *d)
{
        int iStart;

        static char read_buf[MAX_PROTOCOL_BUFFER]; /* <--- Add this line */
        read_buf[0] = '\0';                        /* <--- Add this line */

        /* Hold horses if pending command already. */
        if (d->incomm[0] != '\0')
                return TRUE;

        /* Check for overflow. 
        iStart = strlen(d->inbuf);
        if ( iStart >= sizeof(d->inbuf) - 10 ) */
        
        /* Check for overflow. */
        iStart = 0;
        if ( strlen(d->inbuf) >= sizeof(d->inbuf) - 10 )
        {
                sprintf(log_buf, "%s input overflow!", d->host);
                log_string(log_buf);
                write_to_descriptor(d->descriptor,
                                    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
                return FALSE;
        }

        /* Snarf input. */
#if defined(macintosh)
        for (; ;)
        {
                int c;
                c = getc(stdin);
                if (c == '\0' || c == EOF)
                        break;
                putc(c, stdout);
               /* if (c == '\r')
                        putc('\n', stdout);
                d->inbuf[iStart++] = c; */
                if ( c == '\r' )
	                putc( '\n', stdout );
	        read_buf[iStart++] = c;
                if (iStart > sizeof(d->inbuf) - 10)
                        break;
        }
#endif

#if defined(MSDOS) || defined(unix)
        for (; ;)
        {
                int nRead;

       /*         nRead = read(d->descriptor, d->inbuf + iStart,
                             sizeof(d->inbuf) - 10 - iStart);
                if (nRead > 0)
                {
                        iStart += nRead;
                        if (d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r')
                                break;
                } */
                nRead = read( d->descriptor, read_buf + iStart,
	        sizeof(read_buf) - 10 - iStart );
                if ( nRead > 0 )
                {
                iStart += nRead;
                if ( read_buf[iStart-1] == '\n' || read_buf[iStart-1] == '\r' )
                        break;
                }
                else if (nRead == 0)
                {
                        return FALSE;
                }
                else if (errno == EWOULDBLOCK || errno == EAGAIN)
                        break;
                else
                        return FALSE;
        }
#endif

     /*   d->inbuf[iStart] = '\0';
        return TRUE; */
        read_buf[iStart] = '\0';
        ProtocolInput( d, read_buf, iStart, d->inbuf );
        return TRUE;
        
}


/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer (DESCRIPTOR_DATA *d)
{
        int i;
        int j;
        int k;

        /*
         * Hold horses if pending command already.
         */
        if (d->incomm[0] != '\0')
                return;

        /*
         * Look for at least one new line.
         */
        for (i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
        {
                if (d->inbuf[i] == '\0')
                        return;
        }

        /*
         * Canonical input processing.
         */
        for (i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
        {
                if (k >= MAX_INPUT_LENGTH - 2)
                {
                        write_to_descriptor(d->descriptor, "Line too long.\n\r", 0);

                        /* skip the rest of the line */
                        for (; d->inbuf[i] != '\0'; i++)
                        {
                                if (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
                                        break;
                        }
                        d->inbuf[i]   = '\n';
                        d->inbuf[i+1] = '\0';
                        break;
                }

                if (d->inbuf[i] == '\b' && k > 0)
                        --k;
                else if (isascii(d->inbuf[i]) && isprint(d->inbuf[i]))
                        d->incomm[k++] = d->inbuf[i];
        }

        /*
         * Finish off the line.
         */
        if (k == 0)
                d->incomm[k++] = ' ';
        d->incomm[k] = '\0';

        /*
         * Deal with bozos with #repeat 1000 ...
         */
        if (k > 1 || d->incomm[0] == '!')
        {
                if (d->incomm[0] != '!' && strcmp(d->incomm, d->inlast))
                {
                        d->repeat = 0;
                }
                else
                {
                        if ( ( ++d->repeat >= 50 ) )
                        {
                                sprintf(log_buf, "%s input spamming!", d->host);
                                log_string(log_buf);
                                write_to_descriptor(d->descriptor,
                                                    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
                                strcpy(d->incomm, "quit");
                        }
                }
        }

        /*
         * Do '!' substitution.
         */
        if (d->incomm[0] == '!')
                strcpy(d->incomm, d->inlast);
        else
                strcpy(d->inlast, d->incomm);

        /*
         * Shift the input buffer.
         */
        while (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
                i++;
        for (j = 0; (d->inbuf[j] = d->inbuf[i+j]) != '\0'; j++)
                ;

        return;
}


/*
 * Low level output function.
 */
bool process_output (DESCRIPTOR_DATA *d, bool fPrompt)
{
        extern bool merc_down;

        /*
         * Bust a prompt.
         */
        if ( d->pProtocol->WriteOOB ) /* <-- Add this, and the ";" and "else" */
        ; /* The last sent data was OOB, so do NOT draw the prompt */
        else if (fPrompt && !merc_down && d->connected == CON_PLAYING)
        {
                if (d->showstr_point)
                {
                        write_to_buffer(d, "[Please type (c)ontinue, (r)efresh, (b)ack, (q)uit, or RETURN]:  ", 0);
                }
                else
                {
                        CHAR_DATA *ch;
                        CHAR_DATA *victim;

                        ch = d->original ? d->original : d->character;

                        /* battle prompt */
                        if ((victim = ch->fighting) != NULL)
                        {
                                int percent;
                                char wound[100];
                                char buf[MAX_STRING_LENGTH];

                                if (victim->max_hit > 0)
                                        percent = victim->hit * 100 / victim->max_hit;
                                else
                                        percent = -1;

                                if ( !IS_INORGANIC( victim ) )
                                {
                                        if (percent >= 100)
                                                sprintf(wound,"is in excellent condition.");
                                        else if (percent >= 90)
                                                sprintf(wound,"has a few scratches.");
                                        else if (percent >= 75)
                                                sprintf(wound,"has some small wounds and bruises.");
                                        else if (percent >= 50)
                                                sprintf(wound,"has quite a few wounds.");
                                        else if (percent >= 30)
                                                sprintf(wound,"has some big nasty wounds and scratches.");
                                        else if (percent >= 15)
                                                sprintf(wound,"looks pretty hurt.");
                                        else if (percent >= 0)
                                                sprintf(wound,"is in awful condition.");
                                        else
                                                sprintf(wound,"is bleeding to death!");
                                }
                                else {

                                        if ( percent >= 100 )
                                                sprintf(wound,"is in perfect condition.");
                                        else if ( percent >=  90 )
                                                sprintf(wound,"is slightly damaged.");
                                        else if ( percent >=  80 )
                                                sprintf(wound,"has a few signs of damage.");
                                        else if ( percent >=  70 )
                                                sprintf(wound,"has noticeable damage.");
                                        else if ( percent >=  60 )
                                                sprintf(wound,"is moderately damaged.");
                                        else if ( percent >=  50 )
                                                sprintf(wound,"has taken a lot of damage.");
                                        else if ( percent >=  40 )
                                                sprintf(wound,"has very significant damage.");
                                        else if ( percent >=  30 )
                                                sprintf(wound,"is very heavily damaged.");
                                        else if ( percent >=  20 )
                                                sprintf(wound,"is ruinously damaged.");
                                        else if ( percent >=  10 )
                                                sprintf(wound,"is on the brink of destruction.");
                                        else
                                                sprintf(wound,"is beyond saving.");
                                }

                                sprintf (buf, "%s %s\n\r",
                                         capitalize_initial (PERS(victim, ch)),
                                         wound);
                                write_to_buffer(d, buf, 0);
                        }

                        if (IS_SET(ch->act, PLR_BLANK))
                                write_to_buffer(d, "\n\r", 2);

                        if (IS_SET(ch->act, PLR_PROMPT))
                                bust_a_prompt(d);

                        if (IS_SET(ch->act, PLR_TELNET_GA))
                                write_to_buffer(d, go_ahead_str, 0);
                }
        }

        /*
         * Short-circuit if nothing to write.
         */
        if (d->outtop == 0)
                return TRUE;

        /*
         * Snoop-o-rama.
         */
        if (d->snoop_by)
        {
                write_to_buffer(d->snoop_by, "% ", 2);
                write_to_buffer(d->snoop_by, d->outbuf, d->outtop);
        }

        /*
         * OS-dependent output.
         */
        if (!write_to_descriptor(d->descriptor, d->outbuf, d->outtop))
        {
                d->outtop = 0;
                return FALSE;
        }
        else
        {
                d->outtop = 0;
                return TRUE;
        }
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


/*
 * Append onto an output buffer.
 */
void write_to_buffer (DESCRIPTOR_DATA *d, const char *txt, int length)
{
        txt = ProtocolOutput( d, txt, &length );  /* <--- GCMP */
        if ( d->pProtocol->WriteOOB > 0 )         /* <--- GCMP */
                --d->pProtocol->WriteOOB;             /* <--- GCMP */
        /*
         * Find length in case caller didn't.
         */
        if (length <= 0)
                length = strlen(txt);

        /*
         * Initial \n\r if needed.
         *
        if (d->outtop == 0 && !d->fcommand) */
        if ( d->outtop == 0 && !d->fcommand && !d->pProtocol->WriteOOB )
        {
                d->outbuf[0]    = '\n';
                d->outbuf[1]    = '\r';
                d->outtop       = 2;
        }

        /*
         * Expand the buffer as needed.
         */
        while (d->outtop + length >= d->outsize)
        {
                char *outbuf;

                outbuf = alloc_mem(2 * d->outsize);
                strncpy(outbuf, d->outbuf, d->outtop);
                free_mem(d->outbuf, d->outsize);
                d->outbuf = outbuf;
                d->outsize *= 2;
        }

        /*
         * Copy.  Modifications sent in by Zavod.
         */
        strncpy(d->outbuf + d->outtop, txt, length);
        d->outtop += length;
        return;
}


/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor (int desc, char *txt, int length)
{
        int iStart;
        int nWrite;
        int nBlock;
        int i;

        if (length <= 0)
                length = strlen(txt);

        for (iStart = 0; iStart < length; iStart += nWrite)
        {
                nBlock = UMIN(length - iStart, 4096);

                /* Added some voodoo to prevent dropping connections due to blocking behaviour; Gezhp 2007 */
                for (i = 0; i < 1024; i++) {
                    if ((nWrite = write(desc, txt + iStart, nBlock)) < 0) {
                        if (errno == EWOULDBLOCK || errno == EAGAIN) {
                            continue;
                        }

                        perror("Write_to_descriptor");
                        return FALSE;
                    }
                    else {
                        break;
                    }
                }
        }

        return TRUE;
}


/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny (DESCRIPTOR_DATA *d, char *argument)
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


        /* Delete leading spaces UNLESS character is writing a note */
        if (d->connected != CON_NOTE_TEXT)
        {
                while (isspace(*argument))
                        argument++;
        }

        /* This is here so we wont get warnings.  ch = NULL anyways - Kahn */
        ch = d->character;

        switch (d->connected)
        {
            default:
                bug("Nanny: bad d->connected %d.", d->connected);
                close_socket(d);
                return;

            case CON_GET_NAME:
                if (argument[0] == '\0')
                {
                        close_socket(d);
                        return;
                }

                argument[0] = UPPER(argument[0]);
                fOld = load_char_obj(d, argument);
                ch = d->character;

                if (!check_parse_name(argument))
                {
                        if (!fOld)
                        {
                                write_to_buffer(d, "Illegal name: try another.\n\rName: ", 0);
                                return;
                        }
                        else
                        {
                                /*
                                 * Trap the "." and ".." logins.
                                 * Chars must be > level 1 here
                                 */
                                if (ch->level < 1)
                                {
                                        write_to_buffer(d, "Illegal name: try another.\n\rName: ", 0);
                                        return;
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
                        return;
                }

                if (check_reconnect(d, argument, FALSE))
                        fOld = TRUE;

                else
                {
                        if (wizlock && ch->level <= LEVEL_HERO)
                        {
                                write_to_buffer(d, "The game is wizlocked.\n\r", 0);
                                close_socket(d);
                                return;
                        }

                        if (ch->level <= numlock && numlock != 0)
                        {
                                write_to_buffer(d, "The game is locked to your level character.\n\r\n\r", 0);
                                close_socket(d) ;
                                return;
                        }
                }

                if (check_playing(d, ch->name))
                {
                        if (d->connected != CON_GET_DISCONNECTION_PASSWORD)
                                close_socket(d);

                        return;
                }

                if (fOld)
                {
                        /* Old player */
                        write_to_buffer(d, "Password: ", 0);
                        /* write_to_buffer(d, echo_off_str, 0); */
                        ProtocolNoEcho( d, true );
                        d->connected = CON_GET_OLD_PASSWORD;
                        return;
                }
                else
                {
                        /* New player */
                        sprintf(buf, "Did I get that right, %s? [y/n] ", argument);
                        write_to_buffer(d, buf, 0);
                        d->connected = CON_CONFIRM_NEW_NAME;
                        return;
                }
                break;

            case CON_GET_OLD_PASSWORD:
#if defined(unix)
                write_to_buffer(d, "\n\r", 2);
#endif
                if (ch->pcdata->pwd != NULL
                    && strlen(ch->pcdata->pwd) > 0
                    && strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
                {
                        write_to_buffer(d, "Wrong password.\n\r", 0);
                        close_socket(d);
                        return;
                }

                write_to_buffer(d, echo_on_str, 0);
                /* ProtocolNoEcho( d, false ); */
                if (check_reconnect(d, ch->name, TRUE))
                        return;

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
                            ProtocolNoEcho( d, true );
                        sprintf( buf, "New character.\n\rGive me a password for %s: ",ch->name );
                        /*
                        sprintf(buf, "\n\rNew character.\n\rEnter a password for %s: %s",
                                ch->name, echo_off_str); */
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
                        return;
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
                        return;
                }

                 /*write_to_buffer(d, echo_on_str, 0); */ 
                 ProtocolNoEcho( d, false );
                write_to_buffer(d, "Do you want to enable colour? [y/n] ",0);
                d->connected = CON_CHECK_ANSI;
                break;

            case CON_CHECK_ANSI:
                /* write_to_buffer(d, echo_on_str, 0); */
                 ProtocolNoEcho( d, false );

                switch (*argument)
                {
                    case 'y': case 'Y':
                        SET_BIT(ch->act, PLR_ANSI);
                        break;

                    case 'n': case 'N':
                        break;

                    default:
                        write_to_buffer(d, "Do you want to enable colour? [y/n] ",0);
                        return;
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
                        return;
                }

                if (ch->race < 1 || ch->race > ( MAX_RACE - 1 ) )
                {
                        send_to_char("\n\r{cPlease choose a race for your character. {C[a-y]{x ", ch);
                        return;
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
                        return;
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
                        return;
                }

                switch (argument[0])
                {
                    case 'm': case 'M': ch->sex = SEX_MALE;    break;
                    case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
                    case 'n': case 'N': ch->sex = SEX_NEUTRAL; break;
                    default:
                        send_to_char("\n\r{Y}rInvalid gender!{x\n\r"
                                     "{cMale, female or neuter? {C[m/f/n]{x ", ch);
                        return;
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
                        return;
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
                        return;
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
                        return;
                }

                write_to_buffer(d, "\n\r", 0);

                /* define the classname for helps. */
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
                        return;
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
                        return;
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

                        /*
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
                MXPSendTag( d, "<VERSION>" );  /* <--- GCMP */
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

                /* write_to_buffer(d, echo_on_str, 0); */
                 ProtocolNoEcho( d, false );

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
} /* nanny mark */


/*
 * Send to one char, new colour version, by Lope.
 */
/*
void send_to_char (const char *txt, CHAR_DATA *ch)
{
        const char    *point;
        char    *point2;
        char    buf[ MAX_STRING_LENGTH*4 ];
        int     skip = 0;

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

        /* If you change these tokens, you should change the color_table_8bit act_codes as well */
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

/*
 * Parse a name for acceptability.
 */
bool check_parse_name (char *name)
{
        int i;

        /*
         * Reserved words.
         */
        if (is_name(name, "all auto imm immortal self someone somebody"))
                return FALSE;

        /*
         * Obscenities
         */
        if (is_name(name, "fuck shit ass arse asshole bitch bastard gay lesbian pussy fucker fucked fart vagina penis cunt faggot nigger"))
                return FALSE;

        /*
         * Length restrictions.
         */
        if (strlen(name) <  3)
                return FALSE;

        if (strlen(name) > 12)
                return FALSE;

        /*
         * Alphanumerics only.
         * Lock out IllIll twits.
         */
        {
                char *pc;
                bool fIll;

                fIll = TRUE;
                for (pc = name; *pc != '\0'; pc++)
                {
                        if (!isalpha(*pc) || *pc == '_')
                                return FALSE;
                        if (LOWER(*pc) != 'i' && LOWER(*pc) != 'l')
                                fIll = FALSE;
                }

                if (fIll)
                        return FALSE;
        }

        /*
         * Prevent players from naming themselves after mobs.
         */
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

        /*
         * No deity or arena bot names either
         */
        for (i = 0; i < NUMBER_DEITIES; i++)
        {
                if (is_name(name, deity_info_table[i].name))
                        return FALSE;
        }

        for (i = 0; i < BOT_TEMPLATE_NUMBER; i++)
        {
                if (is_name(name, bot_template_table[i].name))
                        return FALSE;
        }

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
                                        "password.\n\rPassword for reconnection: ", 0);
                        write_to_buffer(d, echo_off_str, 0);
                        d->connected = CON_GET_DISCONNECTION_PASSWORD;

                        return TRUE;
                }
        }

        return FALSE;
}



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
                write_to_buffer(ch->desc, txt, strlen(txt));
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
                write_to_buffer(ch->desc, txt, strlen(txt));
                return;
        }
}


/*
 * The primary output interface for formatted output.
 */
void act (const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type)
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

        fd = open ("../log/last_command.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

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


/* EOF comm.c */
