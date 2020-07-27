#!/usr/bin/perl

#
#  Watch the DD startup and envy processes to catch crashes.
#  The mud must live!
#
#  Gezhp 2002
#

use strict;
use warnings;

while (1) {
    my $startup = 0;
    my $envy = 0;

    foreach (split /\n+/, `ps x`) {
        $startup = 1    if /start_dd4/;
        $envy = 1       if /envy 8888/;
    }

    #  Run startup script only when both startup and envy aren't running
    system '../src/startup 8888 &' unless ($startup || $envy);
    sleep 60;
}

