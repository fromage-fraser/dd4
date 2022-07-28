#!/usr/bin/perl -w

#
#  Post notice on port; Gezhp 2000
#

use IO::Socket;
use POSIX 'sys_wait_h';

$port = 8888;

$message = "\n\r"
	. "O=====================================================O\n\r"
	. "|          The Dragons Domain MUD has moved!          |\n\r"
	. "|-----------------------------------------------------|\n\r"
	. "|     Point your MUD client to <<mud.nzl.com 8888>.    |\n\r"
	. "|  Don't like DNS?  Try <<203.21.30.41 8888> instead.  |\n\r"
	. "|-----------------------------------------------------|\n\r"
	. "|         See you there... Bring your steel!          |\n\r"
	. "O=====================================================O\n\r"
	. "\n\r";

sub REAPER {
	1 until (-1 == waitpid(-1, WNOHANG));
	$SIG{CHLD} = \&REAPER;
}

$server = IO::Socket::INET->new(LocalPort => $port, Type => SOCK_STREAM,
		Reuse => 1, Listen => 10)
	|| die "Couldn't set up socket: $!\n";

$SIG{CHLD} = \&REAPER;

while (accept(CLIENT, $server)) {
	next if $pid = fork;
	die "fork: $!" unless defined $pid;
	close($server);
	print CLIENT $message;
	exit;
}
continue {
	close(CLIENT);
}


