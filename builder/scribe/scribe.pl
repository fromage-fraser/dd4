#! /usr/bin/perl -w

#############################################################################
#
#  scribe.pl
#  DD4 Area File Scribe
#
#  Format area files for DD4 Envy-style Mud from simplified text source.
#  Refer to 'scribe.docx' for information about how to use this tool to
#  create area files for DD4.

use strict;
use warnings;
print "DD4 Area File Scribe version 0.7\n";


#############################################################################
#
#  Get source and destination files
#

my ($source_file, $destination_file) = @ARGV;

if (!$source_file || !$destination_file) {
    print "Usage: scribe.pl <source file> <destination file>\n";
    exit 1;
}

if ($source_file eq $destination_file) {
    print "Destination file is the same as source file.\n";
    exit 1;
}


#############################################################################
#
#  Flag, keyword and label definitions
#

my %mob_act = (
        none               => 0,
        zero               => 0,
        sentinel           => 2,
        scavenger          => 4,
        questmaster        => 8,
        aggressive         => 32,
        aggro              => 32,
        stay_area          => 64,
        wimpy              => 128,
        pet                => 256,
        no_quest           => 512,
        practice           => 1024,
        regenerator        => 2048,
        regenerate         => 2048,
        no_charm           => 4096,
        healer             => 8192,
        famous             => 16384,
        lose_fame          => 32768,
        wizinvis           => 65536,
        mount              => 131072,
        tinker             => 262144,
        tinker             => 262144,
        banker             => 524288,
        identify           => 1048756,
        die_if_master_gone => 2097152,
        clan_guard         => 4194304,
        no_summon          => 8388608,
        no_experience      => 16777216,
        no_xp              => 16777216,
        no_exp             => 16777216,
        no_heal            => 33554432,
        cannot_fight       => 67108864,
        cant_fight         => 67108864,
        no_fight           => 67108864,
        objectlike         => 134217728,
        invulnerable       => 268435456,
        unkillable         => 9223372036854775808,
);

my %mob_aff = (
        none            => 0,
        zero            => 0,
        blind           => 1,
        invisible       => 2,
        invis           => 2,
        detect_evil     => 4,
        det_evil        => 4,
        detect_invis    => 8,
        det_invis       => 8,
        detect_magic    => 16,
        det_magic       => 16,
        detect_hidden   => 32,
        det_hidden      => 32,
        hold            => 64,
        sanctuary       => 128,
        sanc            => 128,
        faerie_fire     => 256,
        infrared        => 512,
        cursed          => 1024,
        fireshield      => 2048,
        flaming         => 2048,
        poisoned        => 4096,
        poison          => 4096,
        protection      => 8192,
        meditating      => 16384,
        meditate        => 16384,
        sneak           => 32768,
        hide            => 65536,
        hidden          => 65536,
        sleep           => 131072,
        charmed         => 262144,
        fly             => 524288,
        flying          => 524288,
        pass_door       => 1048576,
        passdoor        => 1048576,
        detect_traps    => 2097152,
        det_traps       => 2097152,
        battle_aura     => 4194304,
        detect_sneak    => 8388608,
        det_sneak       => 8388608,
        globed          => 16777216,
        globe           => 16777216,
        deter           => 33554432,
        swim            => 67108864,
        plague          => 134217728,
        non_corporeal   => 268435456,
        detect_curse    => 536870912,
        det_curse       => 536870912,
        detect_good     => 1073741824,
        det_good        => 1073741824,
        swallowed       => 2147483648,
        no_recall       => 4294967296,
        DOT             => 8589934592,
        prone           => 17179869184,
        dazed           => 34359738368,
        confusion       => 68719476736,
        slow            => 9223372036854775808,
);

my %mob_bf = (
        none      => 0,
        zero      => 0,
        no_head   => 1,
        no_eyes   => 2,
        no_arms   => 4,
        no_legs   => 8,
        no_heart  => 16,
        no_speech => 32,
        no_corpse => 64,
        huge      => 128,
        inorganic => 256,
        has_tail  => 512,
);

my @mob_spec = qw/

    spec_breath_any	        spec_cast_druid	        spec_warrior	          spec_aboleth
    spec_breath_acid	    spec_cast_water_sprite	spec_vampire	          spec_laghathti
    spec_breath_fire	    spec_cast_psionicist	spec_cast_archmage	      spec_superwimpy
    spec_breath_frost	    spec_cast_undead	    spec_cast_priestess	      spec_uzollru
    spec_breath_gas	        spec_executioner	    spec_mast_vampire	      spec_sahuagin_baron
    spec_breath_lightning	spec_fido	            spec_bloodsucker	      spec_sahuagin_prince
    spec_breath_steam	    spec_guard	            spec_spectral_minion	  spec_green_grung
    spec_cast_adept	        spec_janitor	        spec_celestial_repairman  spec_sahuagin_infantry
    spec_cast_hooker	    spec_poison	            spec_sahuagin	          spec_sahuagin_cavalry
    spec_buddha	            spec_repairman	        spec_evil_evil_gezhp	  spec_sahuagin_guard
    spec_kungfu_poison	    spec_thief	            spec_demon	              spec_sahuagin_lieutenant
    spec_clan_guard	        spec_bounty	            spec_cast_electric	      spec_sahuagin_cleric
    spec_cast_cleric	    spec_grail	            spec_small_whale	      spec_sahuagin_high_cleric
    spec_cast_judge	        spec_cast_orb	        spec_large_whale	      spec_red_grung
    spec_cast_mage	        spec_assassin	        spec_kappa                spec_blue_grung
    spec_purple_grung	    spec_orange_grung	    spec_gold_grung
/;

my %obj_ex = (
        none         => 0,
        zero         => 0,
        glow         => 1,
        hum          => 2,
        ego          => 4,
        anti_ranger  => 8,
        evil         => 16,
        invis        => 32,
        magic        => 64,
        no_drop      => 128,
        'bless'      => 256,
        blessed      => 256,
        anti_good    => 512,
        anti_evil    => 1024,
        anti_neutral => 2048,
        no_remove    => 4096,
        inventory    => 8192,
        poisoned     => 16384,
        poison       => 16384,
        anti_mage    => 32768,
        anti_cleric  => 65536,
        anti_thief   => 131072,
        anti_warrior => 262144,
        anti_psionic => 524288,
        vorpal       => 1048576,
        trap         => 2097152,
        trapped      => 2097152,
        donated      => 4194304,
        blade_thirst => 8388608,
        bladethirst  => 8388608,
        sharp        => 16777216,
        forged       => 33554432,
        body_part    => 67108864,
        lance        => 134217728,
        anti_brawler => 268435456,
        anti_shifter => 536870912,
        bow          => 1073741824,
        anti_smithy  => 17179869184,
        deployed     => 34359738368,
        rune         => 68719476736,
        pure         => 137438953472,
        steady       => 274877906944,
        cursed       => 2305843009213693952,
);

my %obj_we = (
        none       => 0,
        zero       => 0,
        take       => 1,
        finger     => 2,
        neck       => 4,
        body       => 8,
        head       => 16,
        legs       => 32,
        feet       => 64,
        hands      => 128,
        arms       => 256,
        shield     => 512,
        about_body => 1024,
        waist      => 2048,
        wrist      => 4096,
        wield      => 8192,
        hold       => 16384,
        float      => 32768,
        pouch      => 65536,
        ranged     => 131072,
);

my %obj_container = (
        none      => 0,
        closeable => 1,
        pickproof => 2,
        closed    => 4,
        locked    => 8,
);

my @obj_app = qw/
        none            str             dex                 int
        wis             con             sex                 class
        level           age             height              weight
        mana            hp              move                gold
        exp             ac              hitroll             damroll
        save_para       save_rod        save_petri          save_breath
        save_spell      sanc            sneak               fly
        invis           det_invis       det_hidden          flaming
        protect         pass_door       globe               dragon_aura
        resist_heat     resist_cold     resist_lightning    resist_acid
        breathe_water   balance         set_uncommon        set_rare
        set_epic        set_legendary   strengthen          engraved
        serrated        inscribed       crit                swift
/;

my @obj_ty = qw/
        ?               light                   scroll              wand
        staff           weapon                  digging_implement   hoard
        treasure        armour                  potion              ?
        furniture       trash                   ?                   container
        ?               drink_container         key                 food
        money           ?                       boat                npc_corpse
        pc_corpse       fountain                pill                climbing_eq
        paint           ?                       anvil               auction_ticket
        clan            portal                  poison_powder       lockpick
        instrument      armourers_hammer        mithril             whetstone
        craft           spellcraft              turret_module       forge
        arrestor_unit   driver_unit             reflector_unit      shield_unit
        turret          defensive_turret_module combat_pulse        defensive_pulse
        pipe            pipe_cleaner            smokeable           remains
/;

my @obj_weapon = qw/
        hit         slice       stab        slash
        whip        claw        blast       pound
        crush       grep        bite        pierce
        suction     chop        rake        swipe
        sting       scoop       mash        hack
/;

my @bp_location = qw/
        default     head        eye         arm
        leg         heart       tail        torso
/;

my @mob_sx = qw/
        neuter      male        female
/;

my @mob_rank = qw/
        common      rare        elite       boss        world
/;

my @obj_liquids = qw/
        water       beer           wine          ale
        dark_ale    whisky         lemonade      firebreather
        local       slime_mould    milk          tea
        coffee      blood          salt_water    cola
/;

my @room_st = qw/
        inside              city        field       forest
        hills               mountain    water_swim  water_no_swim
        underwater          air         desert      swamp
        underwater_ground
/;

my @exit_ds = qw/
        open        close       lock
/;

my @exit_lo = qw/
        none                door        pick            bash
        pick_bash           pass        pick_pass       bash_pass
        pick_bash_pass      wall        door_secret     door_secret_pbp
        secret
/;

my %exit_lookup = (
        n => 0, e => 1, s => 2, w => 3, u => 4, d => 5
);

my %room_rf = (
        none            => 0,
        zero            => 0,
        dark            => 1,
        no_mob          => 4,
        indoors         => 8,
        inside          => 8,
        vault           => 16,
        craft           => 128,
        crafting        => 128,
        spellcraft      => 256,
        spellcrafting   => 256,
        private         => 512,
        safe            => 1024,
        solitary        => 2048,
        pet_shop        => 4096,
        no_recall       => 8192,
        norecall        => 8192,
        silence         => 16384,
        arena           => 32768,
        healing         => 65536,
        freezing        => 131072,
        burning         => 262144,
        no_mount        => 524288,
        toxic           => 1048576,
        no_drop         => 9223372036854775808,
        nodrop          => 9223372036854775808,

);

my @wear_loc = qw/
        light       finger1     finger2     neck1
        neck2       body        head        legs
        feet        hands       arms        shield
        about       waist       wrist1      wrist2
        wield       hold        dual        float
        pouch       ranged
/;

my @trap_dam = qw/
        sleep       teleport    fire        cold
        acid        energy      blunt       pierce
        slash       poison      snare       curse
        hex         spirit
/;

my %trap_trig = (
        move   => 1,
        object => 2,
        room   => 4,
        north  => 8,
        south  => 16,
        east   => 32,
        west   => 64,
        up     => 128,
        down   => 256,
        'open' => 512,
);

my %game_str = (
        ult         => 'game_u_l_t',
        highdice    => 'game_high_dice',
        seven       => 'game_seven',
);

my @area_specials = qw /
        school      no_quest    hidden      safe
        no_teleport no_magic    exp_mod     reset_msg
/;



#############################################################################
#
#  Process source file
#
#  Slurp up the source file and read in area header, mobile, object, room
#  and reset data.
#

open(my $fh, '<', $source_file) or die "Couldn't open the source file `$source_file': $!\n";
my @source = <$fh>;
close $fh;

print "Reading source...\n";

my ($fatal, $line, $msg, %area, @recall, @special, @mobs, @objs, @rooms, @addmobs, @helps, @addobjs, @shops, @games);

while (1) {
    my $next = shift @source;
    last unless $next;
    $line++;
    next unless $next =~ /^\$(\w+)/;
    my $header = $1;
    chomp $header;


    #  Area information

    if ($header eq 'area') {
        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: area: $data\n" if $data;
                next;
            }

            next if &add_field_data(\%area, $field, $data, 'bv au ti ls us le ue');
            print "    line $line: area: unknown field '$field'\n";
        }
    }


    #  Recall information

    elsif ($header eq 'recall') {
        my %recall;
        $recall{'line'} = $line;

        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: recall: $data\n" if $data;
                next;
            }

            next if &add_field_data(\%recall, $field, $data, 'rl');
            print "    line $line: recall: unknown field '$field'\n";
        }

        push @recall, [ %recall ];
    }


    # Area special information

    elsif ($header eq 'special') {
        my %special;
        $special{'line'} = $line;

        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: special: $data\n" if $data;
                next;
            }

            next if &add_field_data(\%special, $field, $data, 'af xp rm');
            print "    line $line: special: unknown field '$field'\n";
        }

        push @special, [ %special ];
    }


    #  Recall information

    elsif ($header eq 'recall') {
        my %recall;
        $recall{'line'} = $line;

        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: recall: $data\n" if $data;
                next;
            }

            next if &add_field_data(\%recall, $field, $data, 'rl');
            print "    line $line: recall: unknown field '$field'\n";
        }

        push @recall, [ %recall ];
    }


    # Area special information

    elsif ($header eq 'special') {
        my %special;
        $special{'line'} = $line;

        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: special: $data\n" if $data;
                next;
            }

            next if &add_field_data(\%special, $field, $data, 'af xp rm');
            print "    line $line: special: unknown field '$field'\n";
        }

        push @special, [ %special ];
    }


    #  One help entry

    elsif ($header eq 'help') {
        my %help;
        $help{'line'} = $line;

        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if ($field eq 'he') {
                my $start = $line;
                my ($msg, $error) = &get_text_block(\@source);

                if (!$msg) {
                    print "    line $start: help: $error\n";
                    $fatal = 1;
                    last;
                }

                if (!$data) {
                    print "    line $start: help: keywords undefined\n";
                    $fatal = 1;
                }

                $data = "\U$data";
                $help{'he'} = "$data~\n$msg~\n";
                next;
            }

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: help: $data\n" if $data;
                next;
            }

            next if &add_field_data(\%help, $field, $data, 'lv');
            print "    line $line: help: unknown field '$field'\n";
        }

        push @helps, [ %help ];
    }


    #  One mobile

    elsif ($header eq 'mob' || $header eq 'mobile') {
        my %mob;
        $mob{'line'} = $line;

        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if ($field eq 'de') {
                my $start = $line;
                my ($msg, $error) = &get_text_block(\@source);

                if (!$msg) {
                    print "    line $start: mob: $error\n";
                    $fatal = 1;
                    last;
                }
                else {
                    $mob{'de'} = $msg;
                }
                next;
            }

            if ($field eq 'mp') {
                my $start = $line;
                my ($msg, $error) = &get_text_block(\@source);

                if (!$msg) {
                    print "    line $start: mob: $error\n";
                    $fatal = 1;
                    last;
                }
                else {
                    push @{$mob{'mp'}}, "$data~\n$msg~\n";
                }
                next;
            }

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: mob: $data\n" if $data;
                next;
            }

            if ($field eq 'te') {
                if (!$data) {
                    print "    line $line: mob: field 'te' empty\n";
                }
                else {
                    push @{$mob{'te'}}, $data;
                }
                next;
            }

            next if &add_field_data(\%mob, $field, $data, 'sp vn nm sh lo lv act aff bf sx al rnk');
            print "    line $line: mob: unknown field '$field'\n";
        }

        push @mobs, [ %mob ];
    }


    #  One mobile reset

    elsif ($header eq 'addmob' || $header eq 'addmobile') {
        my %mob;
        $mob{'line'} = $line;

        ADDMOB:
        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: mob: $data\n" if $data;
                next;
            }

            foreach (qw/inv rm/) {
                if ($field eq $_) {
                    if (!$data && $data ne '0') {
                        print "    line $line: addmob: field '$_' empty\n";
                    }
                    else {
                        push @{$mob{$_}}, $data;
                    }
                    next ADDMOB;
                }
            }

            next if &add_field_data(\%mob, $field, $data,
                    'mb num light finger1 finger2 neck1 neck2 body head legs feet hands arms shield about waist wrist1
                     wrist2 wield hold dual float pouch ranged');
            print "    line $line: addmob: unknown field '$field'\n";
        }

        push @addmobs, [ %mob ];
    }


    #  One object

    elsif ($header eq 'obj' || $header eq 'object') {
        my %obj;
        $obj{'line'} = $line;

        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if ($field eq 'ed') {
                my $start = $line;
                my ($msg, $error) = &get_text_block(\@source);

                if (!$msg) {
                    print "    line $start: obj: $error\n";
                    $fatal = 1;
                    last;
                }
                else {
                    push @{$obj{'ed'}}, "$data~\n$msg~\n";
                }
                next;
            }

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: obj: $data\n" if $data;
                next;
            }

            if ($field eq 'ap') {
                if (!$data) {
                    print "    line $line: obj: field 'ap' empty\n";
                }
                else {
                    push @{$obj{'ap'}}, $data;
                }
                next;
            }

            next if &add_field_data(\%obj, $field, $data, 'vn nm sh lo ty ex we wg osl v0 v1 v2 v3 trd trt trc mi');
            print "    line $line: obj: unknown field '$field'\n";
        }

        push @objs, [ %obj ];
    }


    #  One object reset

    elsif ($header eq 'addobj' || $header eq 'addobject') {
        my %obj;
        $obj{'line'} = $line;

        ADDOBJ:
        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: obj: $data\n" if $data;
                next;
            }

            foreach (qw/con rm/) {
                if ($field eq $_) {
                    if (!$data) {
                        print "    line $line: addobj: field '$_' empty\n";
                    }
                    else {
                        push @{$obj{$_}}, $data;
                    }
                    next ADDOBJ;
                }
            }

            next if &add_field_data(\%obj, $field, $data, 'ob num lv rm');
            print "    line $line: addobj: unknown field '$field'\n";
        }

        push @addobjs, [ %obj ];
    }


    #  One room

    elsif ($header eq 'room') {
        my %room;
        $room{'line'} = $line;

        ROOM:
        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            foreach my $next (qw/de nde sde wde ede ude dde/) {
                if ($field eq $next) {
                    my $start = $line;
                    my ($msg, $error) = &get_text_block(\@source);

                    if (!$msg) {
                        print "    line $start: room: $error\n";
                        $fatal = 1;
                        next ROOM;
                    }
                    else {
                        $room{$next} = $msg;
                    }
                    next ROOM;
                }
            }

            if ($field eq 'ed') {
                my $start = $line;
                my ($msg, $error) = &get_text_block(\@source);

                if (!$msg) {
                    print "    line $start: room: $error\n";
                    $fatal = 1;
                    last;
                }
                else {
                    push @{$room{'ed'}}, "$data~\n$msg~\n";
                }
                next;
            }

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: room: $data\n" if $data;
                next;
            }

            next if &add_field_data(\%room, $field, $data,
                    'vn rf st nm n s e w u d rnd nnm snm enm wnm unm dnm ulo dlo wlo elo nlo slo nke ske wke eke uke dke
                     nds sds eds wds uds dds');
            print "    line $line: room: unknown field '$field'\n";
        }

        push @rooms, [ %room ];
    }


    #  One shop

    elsif ($header eq 'shop') {
        my %shop;
        $shop{'line'} = $line;

        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: shop: $data\n" if $data;
                next;
            }

            next if &add_field_data(\%shop, $field, $data, 'vn t1 t2 t3 t4 t5 ps pb oh ch');
            print "    line $line: shop: unknown field '$field'\n";
        }

        push @shops, [ %shop ];
    }

    #  One game

    elsif ($header eq 'game') {
        my %game;
        $game{'line'} = $line;

        while (@source) {
            my ($field, $data) = &get_field_data(\@source);

            if (!$field) {
                last if $data eq 'BREAK';
                print "    line $line: game: $data\n" if $data;
                next;
            }

            next if &add_field_data(\%game, $field, $data, 'mb gm br mw ch');
            print "    line $line: game: unknown field '$field'\n";
        }

        push @games, [ %game ];
    }


    #  Unknown header

    else {
        print "    line $line: unknown header '$header'\n";
    }
}

if ($fatal) {
    print "\nAborted\n";
    exit 0;
}


#############################################################################
#
#  Subroutine:  Get field and data keys from source file
#

sub get_field_data(\@\$) {
    my $source = shift;
    my ($key, $value);

    while (1) {
        my $next = shift @$source;
        return (0, 'BREAK') if !$next;

        if ($next =~ /(^\$\w+)/) {
            unshift @source, $1;
            return (0, 'BREAK');
        }

        $line++;
        next if $next =~ /^\s*$/;
        chomp $next;
        return ($1, '') if $next =~ /^(\w+)\s*$/;
        next unless $next =~ /^(\w+)\s+(.*\S)/;
        ($key, $value) = ($1, $2);
        $value =~ s/\~//g;
        return ($key, $value);
    }
}


#############################################################################
#
#  Subroutine:  Add a field/data pair during first pass through source file
#

sub add_field_data(\%$$$) {
    my ($var, $field, $data, $list) = @_;

    foreach (split /\s+/, $list) {
        if ($_ eq $field) {
            $$var{$field} = $data;
            return 1;
        }
    }

    return 0;
}


#############################################################################
#
#  Subroutine:  Get a block of text for long and extra descriptions
#

sub get_text_block(\@) {
    my $source = shift;
    my $text = '';

    while (1) {
        my $next = shift @$source;
        return (0, 'unterminated text block') unless $next;

        if ($next =~ /(^\$\w+)/) {
            unshift @source, $1;
            return (0, 'header found in text block');
        }

        $line++;

        if ($next =~ /^~/) {
            return ($text, 0) unless $text eq '';
            return (0, 'text block empty');
        }

        $next =~ s/\~/\^/g;
        $text .= $next;
    }
}


#############################################################################
#
#  Verify data read from source
#
#  Check for missing fields; convert flag and variable keywords to numerical
#  values; check validity of some variables; format data for write to area
#  file.
#

print "Verifying data...\n";

my ($area_errors, %recall_errors, %special_errors, %mob_errors, %mob_vnums, %obj_errors, %obj_vnums, @specials,
        %room_errors, %room_vnums, @resets, %addmob_errors, %room_names,
        %mob_names, %obj_names, %help_errors, %addobj_errors, %game_errors, %shop_errors);


#  Area header

{
    foreach (qw/ti au/) {
        if ($msg = &check_field_defined(\%area, $_)) {
            print "    area: $msg\n";
            $area_errors++;
        }
    }

    foreach (qw/ue le bv/) {
        if ($msg = &check_field_number_range(\%area, $_, 0, 'none')) {
            print "    area: $msg\n";
            $area_errors++;
        }
    }

    foreach (qw/us ls/) {
        if ($msg = &check_field_number_range(\%area, $_, -100, 'none')) {
            print "    area: $msg\n";
            $area_errors++;
        }
    }

    $area{'bv'} = 0 unless exists $area{'bv'};
}


#  Recall header

foreach (0 .. $#recall) {
    my %recall = @{$recall[$_]};
    my $err = "    recall, line $recall{'line'}:";

    foreach (qw/rl/) {
        if ($msg = &check_field_number_range(\%recall, $_, 0, 'none')) {
            print "$err $msg\n";
            $recall_errors{$recall{'line'}}++;
        }
    }
    $recall[$_] = [ %recall ];
}



#  Area special header

foreach (0 .. $#special) {
    my %special = @{$special[$_]};
    my $err = "    special, line $special{'line'}:";

    if (exists($special{'af'})) {
        if ($msg = &check_keyword_list(\%special, 'af', \@area_specials)) {
               print "$err $msg\n";
               $special_errors{$special{'line'}}++;
        }
    }

    if (exists($special{'xp'})) {
        if ($msg = &check_field_number_range(\%special, 'xp', 0, 'none')) {
            print "$err $msg\n";
            $special_errors{$special{'line'}}++;
        }
    }

    $special[$_] = [ %special ];
}


#  Recall header

foreach (0 .. $#recall) {
    my %recall = @{$recall[$_]};
    my $err = "    recall, line $recall{'line'}:";

    foreach (qw/rl/) {
        if ($msg = &check_field_number_range(\%recall, $_, 0, 'none')) {
            print "$err $msg\n";
            $recall_errors{$recall{'line'}}++;
        }
    }
    $recall[$_] = [ %recall ];
}



#  Area special header

foreach (0 .. $#special) {
    my %special = @{$special[$_]};
    my $err = "    special, line $special{'line'}:";

    if (exists($special{'af'})) {
        if ($msg = &check_keyword_list(\%special, 'af', \@area_specials)) {
               print "$err $msg\n";
               $special_errors{$special{'line'}}++;
        }
    }

    if (exists($special{'xp'})) {
        if ($msg = &check_field_number_range(\%special, 'xp', 0, 'none')) {
            print "$err $msg\n";
            $special_errors{$special{'line'}}++;
        }
    }

    $special[$_] = [ %special ];
}


#  Helps

foreach (0 .. $#helps) {
    my %help = @{$helps[$_]};
    my $err = "    help, line $help{'line'}:";

    $help{'lv'} = 0 if (!exists $help{'lv'} || $help{'lv'} eq '');

    if ($msg = &check_field_number_range(\%help, 'lv', -1, 'none')) {
        print "$err $msg\n";
        $help_errors{$help{'line'}}++;
    }

    if ($msg = &check_field_defined(\%help, 'he')) {
        print "$err $msg\n";
        $help_errors{$help{'line'}}++;
    }

    $helps[$_] = [ %help ];
}


#  Mobiles

foreach (0 .. $#mobs) {
    my %mob = @{$mobs[$_]};
    my $err = "    mob, line $mob{'line'}:";

    foreach (qw/act aff bf sx al rnk/) {
        $mob{$_} = 0 if (!exists $mob{$_} || $mob{$_} eq '');
    }

    foreach (qw/nm sh lo de/) {
        if ($msg = &check_field_defined(\%mob, $_)) {
            print "$err $msg\n";
            $mob_errors{$mob{'line'}}++;
        }
    }

    if ($msg = &check_field_number_range(\%mob, 'vn', 0, 'none')) {
        print "$err $msg\n";
        $mob_errors{$mob{'line'}}++;
    }
    elsif (++$mob_vnums{$mob{'vn'}} > 1) {
        print "$err vnum '$mob{'vn'}' already in use\n";
        $mob_errors{$mob{'line'}}++;
    }

    if ($msg = &check_field_number_range(\%mob, 'lv', 1, 'none')) {
        print "$err $msg\n";
        $mob_errors{$mob{'line'}}++;
    }

    if ($msg = &check_field_number_range(\%mob, 'al', -1000, 1000)) {
        print "$err $msg\n";
        $mob_errors{$mob{'line'}}++;
    }

    if ($msg = &get_single_flag(\%mob, 'sx', \@mob_sx)) {
        print "$err $msg\n";
        $mob_errors{$mob{'line'}}++;
    }

    if ($msg = &get_single_flag(\%mob, 'rnk', \@mob_rank)) {
        print "$err $msg\n";
        $mob_errors{$mob{'line'}}++;
    }

    if ($msg = &get_multiple_flags(\%mob, 'bf', \%mob_bf, '')) {
        print "$err $msg\n";
        $mob_errors{$mob{'line'}}++;
    }

    if ($msg = &get_multiple_flags(\%mob, 'act', \%mob_act, '')) {
        print "$err $msg\n";
        $mob_errors{$mob{'line'}}++;
    }

    if ($msg = &get_multiple_flags(\%mob, 'aff', \%mob_aff, '')) {
        print "$err $msg\n";
        $mob_errors{$mob{'line'}}++;
    }

    if (exists $mob{'sp'}) {
        my $found = 0;

        foreach (@mob_spec) {
            if ($mob{'sp'} eq $_) {
                push @specials, "M " . ($mob{'vn'} + $area{'bv'})
                        . " $mob{'sp'} \t$mob{'sh'}\n";
                $found = 1;
                last;
            }
        }

        if (!$found) {
            print "$err invalid special function: $mob{'sp'}\n";
            $mob_errors{$mob{'line'}}++;
        }
    }

    if (exists $mob{'te'}) {
        my @tmp;

        foreach my $next (@{$mob{'te'}}) {
            if ($next =~ /(\S+)\s+(.*)/) {
                my ($percent, $skill) = ($1, $2);

                if (!&is_number($percent)) {
                    print "$err field 'te': percentage value not a number: $percent\n";
                    $mob_errors{$mob{'line'}}++;
                    next;
                }

                if ($percent < 0) {
                    print "$err field 'te': percentage out of range: $percent\n";
                    $mob_errors{$mob{'line'}}++;
                    next;
                }

                $skill =~ s/\'//g;
                push @tmp, "& $percent '$skill'\n";
            }
            else {
                print "$err field 'te': invalid or incomplete\n";
                $mob_errors{$mob{'line'}}++;
            }
        }
        $mob{'te'} = [ @tmp ];
    }

    $mob_names{$mob{'vn'}} = $mob{'sh'}
            unless $mob_errors{$mob{'line'}};

    $mobs[$_] = [ %mob ];
}


#  Objects

foreach (0 .. $#objs) {
    my %obj = @{$objs[$_]};
    my $err = "    obj, line $obj{'line'}:";

    foreach (qw/we ex osl mi/) {
        $obj{$_} = 0 if (!exists $obj{$_} || $obj{$_} eq '');
    }

    foreach (qw/v0 v1 v2 v3/) {
        $obj{$_} = '' unless exists $obj{$_};
    }

    foreach (qw/nm sh lo/) {
        if ($msg = &check_field_defined(\%obj, $_)) {
            print "$err $msg\n";
            $obj_errors{$obj{'line'}}++;
        }
    }

    if ($msg = &check_field_number_range(\%obj, 'vn', 0, 'none')) {
        print "$err $msg\n";
        $obj_errors{$obj{'line'}}++;
    }
    elsif (++$obj_vnums{$obj{'vn'}} > 1) {
        print "$err vnum '$obj{'vn'}' already in use\n";
        $obj_errors{$obj{'line'}}++;
    }

    if ($msg = &check_field_number_range(\%obj, 'wg', 0, 'none')) {
        print "$err $msg\n";
        $obj_errors{$obj{'line'}}++;
    }

    if ($msg = &get_single_flag(\%obj, 'ty', \@obj_ty)) {
        print "$err $msg\n";
        $obj_errors{$obj{'line'}}++;
    }

    if ($msg = &get_multiple_flags(\%obj, 'ex', \%obj_ex, '')) {
        print "$err $msg\n";
        $obj_errors{$obj{'line'}}++;
    }
    elsif (&add_bits($obj{'ex'}) & $obj_ex{'trap'}) {
        if ($msg = &get_single_flag(\%obj, 'trd', \@trap_dam)) {
            print "$err $msg\n";
            $obj_errors{$obj{'line'}}++;
        }
        else {
            $obj{'trd'}--;
        }

        if ($msg = &get_multiple_flags(\%obj, 'trt', \%trap_trig)) {
            print "$err $msg\n";
            $obj_errors{$obj{'line'}}++;
        }

        if ($msg = &check_field_number_range(\%obj, 'trc', 0, 'none')) {
            print "$err $msg\n";
            $obj_errors{$obj{'line'}}++;
        }
    }

    if ($msg = &get_multiple_flags(\%obj, 'we', \%obj_we, '')) {
        print "$err $msg\n";
        $obj_errors{$obj{'line'}}++;
    }

    if ($obj{'ty'} && ($msg = &check_object_values(\%obj))) {
        $obj_errors{$obj{'line'}} += $msg;
    }

    if ($msg = &check_object_applies(\%obj)) {
        $obj_errors{$obj{'line'}} += $msg;
    }

    $obj_names{$obj{'vn'}} = $obj{'sh'}
            unless $obj_errors{$obj{'line'}};

    $objs[$_] = [ %obj ];
}


#  Rooms

foreach (0 .. $#rooms) {
    my %room = @{$rooms[$_]};
    my $err = "    room, line $room{'line'}:";

    foreach (qw/rf st/) {
        $room{$_} = 0 if (!exists $room{$_} || $room{$_} eq '');
    }

    foreach (qw/nm de/) {
        if ($msg = &check_field_defined(\%room, $_)) {
            print "$err $msg\n";
            $room_errors{$room{'line'}}++;
        }
    }

    if ($msg = &check_field_number_range(\%room, 'vn', 0, 'none')) {
        print "$err $msg\n";
        $room_errors{$room{'line'}}++;
    }
    elsif (++$room_vnums{$room{'vn'}} > 1) {
        print "$err vnum '$room{'vn'}' already in use\n";
        $room_errors{$room{'line'}}++;
    }

    if ($msg = &get_single_flag(\%room, 'st', \@room_st)) {
        print "$err $msg\n";
        $room_errors{$room{'line'}}++;
    }

    if ($msg = &get_multiple_flags(\%room, 'rf', \%room_rf, '')) {
        print "$err $msg\n";
        $room_errors{$room{'line'}}++;
    }

    if (exists $room{'rnd'}) {
        $room{'rnd'} = 4 if $room{'rnd'} eq '2d';
        $room{'rnd'} = 6 if $room{'rnd'} eq '3d';

        if ($msg = &check_field_number(\%room, 'rnd', 0, 6)) {
            print "$err $msg\n";
            $room_errors{$room{'line'}}++;
        }
        else {
            push @resets, "R 0 " . ($room{'vn'} + $area{'bv'})
                    . " $room{'rnd'}\trandomise exits in $room{'nm'}\n";
        }
    }


    #  Exits

    foreach my $exit (qw/n s e w u d/) {
        if (exists $room{$exit}) {
            $room{$exit . 'lo'} = 0
                    unless exists $room{$exit . 'lo'};

            $room{$exit . 'ke'} = -1
                    unless exists $room{$exit . 'ke'};

            foreach (qw/nm de/) {
                $room{"$exit$_"} = '' unless (exists $room{"$exit$_"});
            }

            if ($msg = &check_field_number_range(\%room, $exit, 0, 'none')) {
                print "$err $msg\n";
                $room_errors{$room{'line'}}++;
            }

            if ($msg = &get_single_flag(\%room, $exit . 'lo', \@exit_lo)) {
                print "$err $msg\n";
                $room_errors{$room{'line'}}++;
            }

            if ($msg = &check_field_number_range(\%room, $exit . 'ke', -1, 'none')) {
                print "$err $msg\n";
                $room_errors{$room{'line'}}++;
            }
            elsif ($room{$exit . 'ke'} >= 0) {
                $room{$exit . 'ke'} += $area{'bv'};
            }

            if (exists $room{$exit . 'ds'}) {
                if ($msg = &get_single_flag(\%room, $exit . 'ds', \@exit_ds)) {
                    print "$err $msg\n";
                    $room_errors{$room{'line'}}++;
                    next;
                }
                else {
                    if ($room{$exit . 'lo'} eq 0
                            || $room{$exit . 'lo'} eq 9
                            || $room{$exit . 'lo'} eq 12) {
                        print "$err field '", $exit, "ds' ignored: exit '$exit' is not a door\n";
                        next;
                    }

                    my %dir = (
                            n => 'north', 's' => 'south', e => 'east',
                            w => 'west', u => 'up', d => 'down'
                    );

                    $room{'nm'} = &strip_colour_tags($room{'nm'});

                    push @resets, "D 0 " . ($room{'vn'} + $area{'bv'})
                            . " $exit_lookup{$exit} $room{$exit . 'ds'}\t"
                            . "$exit_ds[$room{$exit . 'ds'}] "
                            . "$dir{$exit} in $room{'nm'}\n";
                }
            }
        }
        else {
            if (exists $room{$exit . 'ds'}) {
                print "$err field '", $exit, "ds' ignored: no '$exit' exit defined\n";
            }
        }
    }

    $room_names{$room{'vn'}} = $room{'nm'}
            unless $room_errors{$room{'line'}};

    $rooms[$_] = [ %room ];
}


#  Room exit destinations

foreach (0 .. $#rooms) {
    my %room = @{$rooms[$_]};

#    foreach my $exit (qw/n s e w u d/) {
#        if (exists $room{$exit} && &is_number($room{$exit})
#                && !$room_vnums{$room{$exit}}) {
#            print "    room, line $room{'line'}: exit '$exit': "
#                    . "destination undefined: $room{$exit}\n";
#            $room_errors{$room{'line'}}++;
#        }
#    }
}


#  Mobile resets

foreach (0 .. $#addmobs) {
    my %mob = @{$addmobs[$_]};
    my $err = "    addmob, line $mob{'line'}:";
    my @eq = ();

    $mob{'num'} = 1 unless exists $mob{'num'};

    if ($msg = &check_field_number_range(\%mob, 'num', 1, 'none')) {
        print "$err $msg\n";
        $addmob_errors{$mob{'line'}}++;
    }

    if ($msg = &check_field_number_range(\%mob, 'mb', 0, 'none')) {
        print "$err $msg\n";
        $addmob_errors{$mob{'line'}}++;
    }
    elsif (!exists $mob_vnums{$mob{'mb'}}) {
        print "$err field 'mb': mob with vnum '$mob{'mb'}' not defined\n";
        $addmob_errors{$mob{'line'}}++;
    }

    foreach my $pos (0 .. $#wear_loc) {
        next unless exists($mob{$wear_loc[$pos]});

        if ($msg = &check_field_number_range(\%mob, $wear_loc[$pos], 0, 'none')) {
            print "$err $msg\n";
            $addmob_errors{$mob{'line'}}++;
        }

        #if (!exists $obj_vnums{$mob{$wear_loc[$pos]}}) {
        #    print "$err field '$wear_loc[$pos]': object with vnum "
        #            . "'$mob{$wear_loc[$pos]}' not defined\n";
        #    $addmob_errors{$mob{'line'}}++;
        #}

        # So equipping objects from other loaded areas is tolerated
        if (!$obj_names{$mob{$wear_loc[$pos]}})
        {
            $obj_names{$mob{$wear_loc[$pos]}} = "[out-of-area item]";
        }

        push @eq, "E 0 " . ($mob{$wear_loc[$pos]} + $area{'bv'})
                . " 0 $pos\t    equip $obj_names{$mob{$wear_loc[$pos]}}"
                . " ($wear_loc[$pos])\n"
                unless ($addmob_errors{$mob{'line'}}
                        || !$obj_names{$mob{$wear_loc[$pos]}});
    }

    while ($mob{'inv'} && @{$mob{'inv'}}) {
        my $next = shift @{$mob{'inv'}};

        if (!&is_number($next)) {
            print "$err field 'inv' not a number: $next\n";
            $addmob_errors{$mob{'line'}}++;
            next;
        }

        if ($next < 0) {
            print "$err field 'inv' out of range: $next\n";
            $addmob_errors{$mob{'line'}}++;
            next;
        }

        #if (!exists $obj_vnums{$next}) {
        #    print "$err field 'inv': object with vnum '$next' not defined\n";
        #    $addmob_errors{$mob{'line'}}++;
        #}

        # So adding objects from other loaded areas is tolerated
        if (!$obj_names{$next})
        {
            $obj_names{$next} = "[out-of-area item]";
        }

        push @eq, "G 0 " . ($next + $area{'bv'}) . " 0\t    carry $obj_names{$next}\n"
                unless $addmob_errors{$mob{'line'}};
    }

    unless (exists $mob{'rm'}) {
        print "$err field 'rm' not defined\n";
        $addmob_errors{$mob{'line'}}++;
    }
    else {
        while ($mob{'rm'} && @{$mob{'rm'}}) {
            my $next = shift @{$mob{'rm'}};
            next unless exists $mob{'mb'};

            if (!&is_number($next)) {
                print "$err field 'rm' not a number: $next\n";
                $addmob_errors{$mob{'line'}}++;
                next;
            }

            if ($next < 0) {
                print "$err field 'rm' out of range: $next\n";
                $addmob_errors{$mob{'line'}}++;
                next;
            }

            if (!exists $room_vnums{$next}) {
                print "$err field 'rm': room with vnum '$next' not defined\n";
                $addmob_errors{$mob{'line'}}++;
                next;
            }

            $room_names{$next} = &strip_colour_tags($room_names{$next});

            push @resets, ("M 0 " . ($mob{'mb'} + $area{'bv'}) . ' '
                    . $mob{'num'} . ' ' . ($next + $area{'bv'}) . "\tadd "
                    . $mob_names{$mob{'mb'}} . " to "
                    . $room_names{$next} . "\n", @eq);
        }
    }
}


#  Object resets

foreach (0 .. $#addobjs) {
    my %obj = @{$addobjs[$_]};
    my $err = "    addobj, line $obj{'line'}:";

    if ($msg = &check_field_number_range(\%obj, 'ob', 0, 'none')) {
        print "$err $msg\n";
        $addobj_errors{$obj{'line'}}++;
    }
    elsif (!exists $obj_vnums{$obj{'ob'}}) {
        print "$err field 'ob': obj with vnum '$obj{'ob'}' not defined\n";
        $addobj_errors{$obj{'line'}}++;
    }

    if (!exists $obj{'rm'} && !exists $obj{'con'}) {
        print "$err no 'rm' or 'con' fields defined\n";
        $addobj_errors{$obj{'line'}}++;
    }
    else {
        while ($obj{'rm'} && @{$obj{'rm'}}) {
            my $next = shift @{$obj{'rm'}};

            if (!&is_number($next)) {
                print "$err field 'rm' not a number: $next\n";
                $addobj_errors{$obj{'line'}}++;
                next;
            }

            if ($next < 0) {
                print "$err field 'rm' out of range: $next\n";
                $addobj_errors{$obj{'line'}}++;
                next;
            }

            if (!exists $room_vnums{$next}) {
                print "$err field 'rm': room with vnum '$next' not defined\n";
                $addobj_errors{$obj{'line'}}++;
                next;
            }

            $obj{'num'} = 1 unless exists $obj{'num'};

            if ($msg = &check_field_number_range(\%obj, 'num', 1, 'none')) {
                print "$err $msg\n";
                $addobj_errors{$obj{'line'}}++;
                next;
            }

            if ($msg = &check_field_number_range(\%obj, 'lv', 0, 'none')) {
                print "$err $msg\n";
                $addobj_errors{$obj{'line'}}++;
                next;
            }

            #
            # Old 'O' style item-to-room reset
            #
            # push @resets, "O 0 " . ($obj{'ob'} + $area{'bv'}) . " 0 "
            #	. ($next + $area{'bv'}) . "\t$obj_names{$obj{'ob'}} "
            #	. "to $room_names{$next}\n"
            #		unless ($addobj_errors{$obj{'line'}}
            #			|| !$obj_names{$obj{'ob'}});
            #

            push @resets, "I " . ($obj{'ob'} + $area{'bv'})
                    . " $obj{'lv'} " . ($next + $area{'bv'})
                    . " $obj{'num'}\t$obj_names{$obj{'ob'}} to $room_names{$next}\n";
        }

        while ($obj{'con'} && @{$obj{'con'}}) {
            my $next = shift @{$obj{'con'}};

            if (!&is_number($next)) {
                print "$err field 'con' not a number: $next\n";
                $addobj_errors{$obj{'line'}}++;
                next;
            }

            if ($next < 0) {
                print "$err field 'con' out of range: $next\n";
                $addobj_errors{$obj{'line'}}++;
                next;
            }

            unless (exists $obj_vnums{$next}) {
                print "$err field 'con': obj with vnum '$next' not defined\n";
                $addobj_errors{$obj{'line'}}++;
                next;
            }

            if (!$addobj_errors{$obj{'line'}} && $obj_names{$next} && $obj{'ob'}
                    && $obj_names{$obj{'ob'}}) {
                push @resets, "P 0 " . ($obj{'ob'} + $area{'bv'}) . " 0 "
                        . ($next + $area{'bv'}) . "\tput $obj_names{$obj{'ob'}} "
                        . "inside $obj_names{$next}\n";
            }
        }
    }
}

#  Games

foreach (0 .. $#games) {
    my %game = @{$games[$_]};
    my $err = "    game, line $game{'line'}:";

    # foreach (qw/t1 t2 t3 t4 t5/) {
    #     $shop{$_} = 0 if (!exists $shop{$_} || $shop{$_} eq '');
    # }

    if ($msg = &check_field_number_range(\%game, 'mb', 0, 'none')) {
        print "$err $msg\n";
        $game_errors{$game{'line'}}++;
    }
    elsif (!$mob_vnums{$game{'mb'}}) {
        print "$err mob with vnum '$game{'mb'}' is undefined\n";
        $game_errors{$game{'line'}}++;
    }

    foreach (qw/br mw ch/) {
        if ($msg = &check_field_number_range(\%game, $_, 0, 'none')) {
            print "$err $msg\n";
            $game_errors{$game{'line'}}++;
        }
    }

    if ($msg = &get_multiple_flags(\%game, 'gm', \%game_str, '')) {
        print "$err $msg\n";
        $game_errors{$game{'line'}}++;
    }

    $games[$_] = "M $game{'mb'} $game{'gm'} \t\t $game{'br'}\t$game{'mw'} $game{'ch'}\n"
            unless $game_errors{$game{'line'}};

    # M 27232 game_seven			1000000   8	0	/* Slymah */
}

#  Shops

foreach (0 .. $#shops) {
    my %shop = @{$shops[$_]};
    my $err = "    shop, line $shop{'line'}:";

    foreach (qw/t1 t2 t3 t4 t5/) {
        $shop{$_} = 0 if (!exists $shop{$_} || $shop{$_} eq '');
    }

    if ($msg = &check_field_number_range(\%shop, 'vn', 0, 'none')) {
        print "$err $msg\n";
        $shop_errors{$shop{'line'}}++;
    }
    elsif (!$mob_vnums{$shop{'vn'}}) {
        print "$err mob with vnum '$shop{'vn'}' is undefined\n";
        $shop_errors{$shop{'line'}}++;
    }

    foreach (qw/pb ps/) {
        if ($msg = &check_field_number_range(\%shop, $_, 0, 'none')) {
            print "$err $msg\n";
            $shop_errors{$shop{'line'}}++;
        }
    }

    foreach (qw/oh ch/) {
        if ($msg = &check_field_number_range(\%shop, $_, 0, 23)) {
            print "$err $msg\n";
            $shop_errors{$shop{'line'}}++;
        }
    }

    foreach (qw/t1 t2 t3 t4 t5/) {
        if ($msg = &get_single_flag(\%shop, $_, \@obj_ty)) {
            print "$err $msg\n";
            $shop_errors{$shop{'line'}}++;
        }
    }

    $shops[$_] = $shop{'vn'} + $area{'bv'} . " $shop{'t1'} $shop{'t2'} "
            . "$shop{'t3'} $shop{'t4'} $shop{'t5'} $shop{'pb'} $shop{'ps'} $shop{'oh'} "
            . "$shop{'ch'}\tshop run by $mob_names{$shop{'vn'}}\n"
            unless $shop_errors{$shop{'line'}};
}

#############################################################################
#
#  Subroutine:  Condense extended number format
#

sub strip_colour_tags {
    my ($input_str) = @_;

    $input_str =~ s/<\d{1,3}>//g;

    return $input_str;
}

sub add_bits($) {
    my @bits = split /\|/, shift;
    my $num = 0;

    foreach (@bits) {
        $num += $_;
    }

    return $num;
}


#############################################################################
#
#  Subroutine:  Check object applies are valid
#

sub check_object_applies(\%) {
    my $obj = shift;
    my $errors = 0;
    my $err = "    obj, line $$obj{'line'}:";
    my @applies = ();

    return 0 unless $$obj{'ap'};

    APPLY:
    while (my $next = shift @{$$obj{'ap'}}) {
        if ($next =~ /(\w+)\s+(.+)/) {
            my ($app, $adj) = ($1, $2);

            foreach my $i (0 .. $#obj_app) {
                if ($app eq $obj_app[$i] || $app eq $i) {
                    if (!&is_number($adj)) {
                        print "$err apply '$app': invalid modifier: '$adj'\n";
                        $errors++;
                    }
                    else {
                        push @applies, "$i $adj\n";
                    }
                    next APPLY;
                }
            }

            print "$err apply '$app': invalid apply type\n";
            $errors++;
            next;
        }

        print "$err field 'ap' invalid or incomplete: $next\n";
        $errors++;
    }

    $$obj{'ap'} = [ @applies ];
    return $errors;
}


#############################################################################
#
#  Subroutine:  Check object values are correct
#

sub check_object_values(\%) {
    my $obj = shift;
    my $errors = 0;
    my $err = "    obj, line $$obj{'line'}:";
    my $msg;

    #  Don't expect to have these two errors; better safe than sorry

    if (!exists $$obj{'ty'}) {
        print "$err check_object_values: 'ty' undefined\n";
        return 1;
    }

    my $type = $$obj{'ty'};

    if (!&is_number($type)) {
        print "$err check_object_values: 'ty' is not a number\n";
        return 1;
    }

    $err .= " type '$obj_ty[$type]':";


    #  Light sources

    if ($type == 1) {

        foreach (qw/v0 v1 v3/) {
            if (!$$obj{$_}) {
                $$obj{$_} = 0;
            }
        }

        if ($msg = &get_single_flag($obj, 'v0', \@bp_location)) {
            print "$err $msg\n";
            $errors++;
        }

        if ($msg = &check_field_number($obj, 'v2')) {
            print "$err $msg\n";
            $errors++;
        }
    }


    #  Scrolls, potions, paints, pills, smokeables

    elsif ($type == 2 || $type == 10 || $type == 28 || $type == 26 || $type == 54) {
        print "$err warning: values 'v1', 'v2' and 'v3' all undefined\n"
                if (!$$obj{'v1'} && !$$obj{'v2'} && !$$obj{'v3'});

        foreach (qw/v1 v2 v3/) {
            $$obj{$_} =~ s/\'//g;
        }

        if ($msg = &check_field_number_range($obj, 'v0', 1, 'none')) {
            print "$err $msg\n";
            $errors++;
        }
    }


    #  Wands, staves

    elsif ($type == 3 || $type == 4) {
        print "$err warning: value 'v3' undefined\n"
                unless $$obj{'v3'};

        $$obj{'v3'} =~ s/\'//g;

        if ($msg = &check_field_number_range($obj, 'v0', 1, 'none')) {
            print "$err $msg\n";
            $errors++;
        }

        if ($msg = &check_field_number_range($obj, 'v1', 0, 'none')) {
            print "$err $msg\n";
            $errors++;
        }

        if ($msg = &check_field_number_range($obj, 'v2', 0, 'none')) {
            print "$err $msg\n";
            $errors++;
        }
    }


    #  Weapons

    elsif ($type == 5) {

        foreach (qw/v0 v1 v2/) {
            if (!$$obj{$_}) {
                $$obj{$_} = 0;
            }
        }

        if ($msg = &get_single_flag($obj, 'v0', \@bp_location)) {
            print "$err $msg\n";
            $errors++;
        }

        if ($msg = &get_single_flag($obj, 'v3', \@obj_weapon)) {
            print "$err $msg\n";
            $errors++;
        }
    }

    # Digging implement

    elsif ($type == 6) {
        foreach (qw/v0 v1 v2 v3/) {
            if (!$$obj{$_}) {
                $$obj{$_} = 0;
            }
        }

        if (!$$obj{'v0'} && !$$obj{'v1'} && !$$obj{'v2'} && !$$obj{'v3'}) {
            print "$err warning: values 'v0' through 'v3' all zero\n";
        }

        foreach (qw/v0 v1 v2 v3/) {
            if ($msg = &check_field_number_range($obj, $_, 0, 'none')) {
                print "$err $msg\n";
                $errors++;
            }
        }
    }

    # Hoards

    elsif ($type == 7) {
        foreach (qw/v0 v1 v2 v3/) {
            if (!$$obj{$_}) {
                $$obj{$_} = 0;
            }
        }

        if (!$$obj{'v0'} && !$$obj{'v1'} && !$$obj{'v2'} && !$$obj{'v3'}) {
            print "$err warning: values 'v0' through 'v3' all zero\n";
        }

        foreach (qw/v0 v1 v2 v3/) {
            if ($msg = &check_field_number_range($obj, $_, 0, 'none')) {
                print "$err $msg\n";
                $errors++;
            }
        }
    }

    # Armour

    elsif ($type == 9) {

        foreach (qw/v0 v1 v2 v3/) {
            if (!$$obj{$_}) {
                $$obj{$_} = 0;
            }
        }

        if ($msg = &get_single_flag($obj, 'v1', \@bp_location)) {
            print "$err $msg\n";
            $errors++;
        }
    }


    #  Containers

    elsif ($type == 15) {
        clear_field($obj, 'v3');

        if ($msg = &check_field_number_range($obj, 'v0', 0, 'none')) {
            print "$err $msg\n";
            $errors++;
        }

        if (&is_number($$obj{'v1'})) {
            if ($msg = &check_field_number_range($obj, 'v1', 0, 15)) {
                print "$err $msg\n";
                $errors++;
            }
        }
        else {
            if ($msg = &get_multiple_flags($obj, 'v1', \%obj_container, 'sum')) {
                print "$err $msg\n";
                $errors++;
            }
        }

        $$obj{'v2'} = -1 if $$obj{'v2'} eq '';

        if (&is_number($$obj{'v2'})) {
            if ($msg = &check_field_number_range($obj, 'v2', -1, 'none')) {
                print "$err $msg\n";
                $errors++;
            }
            else {
                $$obj{'v2'} += $area{'bv'} unless $$obj{'v2'} < 0;
            }
        }
    }


    #  Drink containers

    elsif ($type == 17) {
        if ($msg = &check_field_number_range($obj, 'v0', 0, 'none')) {
            print "$err $msg\n";
            $errors++;
        }

        if ($msg = &check_field_number_range($obj, 'v1', 0, 'none')) {
            print "$err $msg\n";
            $errors++;
        }

        if ($msg = &get_single_flag($obj, 'v2', \@obj_liquids)) {
            print "$err $msg\n";
            $errors++;
        }

        if ($msg = &check_field_number($obj, 'v3')) {
            print "$err $msg\n";
            $errors++;
        }
    }


    #  Food

    elsif ($type == 19) {
        clear_field($obj, 'v1 v2');

        if ($msg = &check_field_number_range($obj, 'v0', 0, 'none')) {
            print "$err $msg\n";
            $errors++;
        }

        if ($msg = &check_field_number($obj, 'v3')) {
            print "$err $msg\n";
            $errors++;
        }
    }


    #  Money

    elsif ($type == 20) {
        foreach (qw/v0 v1 v2 v3/) {
            if (!$$obj{$_}) {
                $$obj{$_} = 0;
            }
        }

        if (!$$obj{'v0'} && !$$obj{'v1'} && !$$obj{'v2'} && !$$obj{'v3'}) {
            print "$err warning: values 'v0' through 'v3' all zero\n";
        }

        foreach (qw/v0 v1 v2 v3/) {
            if ($msg = &check_field_number_range($obj, $_, 0, 'none')) {
                print "$err $msg\n";
                $errors++;
            }
        }
    }

    #  Fountains

    elsif ($type == 25) {

        foreach (qw/v0 v1 v2 v3/) {
            if (!$$obj{$_}) {
                $$obj{$_} = 0;
            }
        }

        # Which liquid?

        if ($msg = &get_single_flag($obj, 'v2', \@obj_liquids)) {
            print "$err $msg\n";
            $errors++;
        }
        # Is it poisoned?

        if ($msg = &check_field_number($obj, 'v3')) {
            print "$err $msg\n";
            $errors++;
        }
    }

    #  Portals

    elsif ($type == 33) {
        if ($msg = &check_field_number_range($obj, 'v0', -1, 'none')) {
            print "$err $msg\n";
            $errors++;
        }
        else {
            $$obj{'v0'} += $area{'bv'};
        }

        if ($msg = &check_field_number_range($obj, 'v1', -1, 'none')) {
            print "$err $msg\n";
            $errors++;
        }
        else {
            $$obj{'v1'} += $area{'bv'};
        }

        if ($msg = &check_field_number_range($obj, 'v2', 0, 'none')) {
            print "$err $msg\n";
            $errors++;
        }

        if ($msg = &check_field_number_range($obj, 'v3', 0, 'none')) {
            print "$err $msg\n";
            $errors++;
        }
    }

    # Key

    elsif ($type == 18) {

        foreach (qw/v0 v1 v2 v3/) {
            if (!$$obj{$_}) {
                $$obj{$_} = 0;
            }
        }
    }

    # Crafting item

    elsif ($type == 40) {

        foreach (qw/v0 v1 v2 v3/) {
            if (!$$obj{$_}) {
                $$obj{$_} = 0;
            }
        }
    }

    # Spellcrafting item

    elsif ($type == 41) {

        foreach (qw/v0 v1 v2 v3/) {
            if (!$$obj{$_}) {
                $$obj{$_} = 0;
            }
        }
    }


    #  The rest

    else {
        clear_field($obj, 'v0 v1 v2 v3');
    }

    return $errors;
}


#############################################################################
#
#  Subroutine:  Clear field
#

sub clear_field(\%$) {
    my ($var, $list) = @_;

    foreach (split /\s+/, $list) {
        $$var{$_} = '' if exists $$var{$_};
    }
}


#############################################################################
#
#  Subroutine:  Find multiple values for a field
#

sub get_multiple_flags(\%$\%$) {
    my ($var, $field, $list, $sum) = @_;
    my %flags = ();

    return $msg if ($msg = &check_field_defined($var, $field));

    if (&is_number($$var{$field})) {
        return "field '$field' out of range: $$var{$field}"
                if ($$var{$field} < 0);
        return 0;
    }

    CHECK:
    foreach my $next (split /\s+/, $$var{$field}) {
        foreach (keys %$list) {
            if ($next eq $_) {
                $flags{$_}++;
                next CHECK;
            }
        }
        return "field '$field' has invalid flag: $next";
    }

    $$var{$field} = '';
    $$var{$field} = 0 if $sum;

    my @sorted = sort {$$list{$a} <=> $$list{$b}} keys %flags;

    foreach (@sorted) {
        if ($sum) {
            $$var{$field} += $$list{$_};
        }
        else {
            $$var{$field} .= '|' if $$var{$field};
            $$var{$field} .= $$list{$_};
        }
    }
}


#############################################################################
#
#  Subroutine:  Find a single value for a field
#

sub get_single_flag(\%$\@) {
    my ($var, $field, $list) = @_;
    return $msg if ($msg = &check_field_defined($var, $field));

    foreach my $i (0 .. $#$list) {
        if ($$var{$field} eq $i || $$var{$field} eq $$list[$i]) {
            $$var{$field} = $i;
            return 0;
        }
    }

    return "field '$field' has invalid value: $$var{$field}";
}

#############################################################################
#
#   Subroutine:   Validate a list of text keywords against an array
#

sub check_keyword_list(\%$) {
    my ($var, $field, $checklist) = @_;
    my $msg;

     # Check if the key exists in the hash.
    if (exists $var->{$field}) {
        # Split the string into an array of words.
        my @words = split(' ', $var->{$field});
        my $word_len = @words;

        if ($word_len < 1) {
            $msg = "Error: key '$field' exists, but has no value";
            return $msg;
        }

        foreach my $word (@words) {
            unless (grep { $_ eq $word } @$checklist) {
                # If the word is not found in the checklist, set the error message.
                $msg = "Error: '$word' is not a valid flag";
                last; # Exit the loop after finding the first unmatched word.
            }
        }
    } else {
        $msg = "Error: The field '$field' does not exist in the hash.";
    }

    return $msg;
}


#############################################################################
#
#  Subroutine:  Check if a field is missing or empty
#

sub check_field_defined(\%$) {
    my ($var, $field) = @_;
    return "field '$field' is missing" unless exists $$var{$field};
    return "field '$field' is empty" if $$var{$field} eq '';
    return 0;
}


#############################################################################
#
#  Check if a field is a number
#

sub check_field_number(\%$) {
    return &check_field_number_range(shift, shift, 'none', 'none');
}



sub check_field_number_range {
    my ($var, $field, $lower, $upper) = @_;
    my $msg;

    return $msg if ($msg = &check_field_defined($var, $field));

    return "field '$field' not a number: $$var{$field}"
            unless (&is_number($$var{$field}));

    return "field '$field' out of range: $$var{$field}"
            if (&is_number($lower) && $lower > $$var{$field}
                    || &is_number($upper) && $upper < $$var{$field});
    return 0;
}

sub is_number($) {
    my $var = shift;
    chomp $var;
    return 1 if ($var =~ /^[-\d]\d*$/ && $var ne '-');
    return 0;
}


#############################################################################
#
#  Report any errors
#

if ($area_errors || keys %recall_errors || keys %special_errors || keys %mob_errors
        || keys %obj_errors || keys %room_errors || keys %addmob_errors
        || keys %addobj_errors || keys %help_errors || keys %shop_errors) {
    sub errors($) {
        my $num = shift;
        my $text = "$num error";
        $text .= 's' unless $num == 1;
        return $text . "\n";
    }

    print "\nErrors:\n";

    if ($area_errors) {
        print "    area: ", errors($area_errors);
    }
    foreach (keys %recall_errors) {
        print "    recall, line $_: ", errors($recall_errors{$_});
    }
    foreach (keys %special_errors) {
        print "    special, line $_: ", errors($special_errors{$_});
    }
    foreach (keys %recall_errors) {
        print "    recall, line $_: ", errors($recall_errors{$_});
    }
    foreach (keys %special_errors) {
        print "    special, line $_: ", errors($special_errors{$_});
    }
    foreach (keys %help_errors) {
        print "    help, line $_: ", errors($help_errors{$_});
    }
    foreach (keys %mob_errors) {
        print "    mob, line $_: ", errors($mob_errors{$_});
    }
    foreach (keys %obj_errors) {
        print "    obj, line $_: ", errors($obj_errors{$_});
    }
    foreach (keys %room_errors) {
        print "    room, line $_: ", errors($room_errors{$_});
    }
    foreach (keys %addmob_errors) {
        print "    addmob, line $_: ", errors($addmob_errors{$_});
    }
    foreach (keys %addobj_errors) {
        print "    addobj, line $_: ", errors($addobj_errors{$_});
    }
    foreach (keys %shop_errors) {
        print "    shop, line $_: ", errors($shop_errors{$_});
    }

    exit 0;
}


#############################################################################
#
#  Move existing destination file
#

if (-e "$destination_file") {
    $| = 1;
    print "Moving file `$destination_file' to `$destination_file.old'... ";
    if (!rename("$destination_file", "$destination_file.old")) {
        print "error: $!\n";
    }
    else {
        print "\n";
    }
}


#############################################################################
#
#  Format area file
#

print "Writing file...\n";

open(AREA, ">$destination_file")
        || die "Couldn't create destination file: $!\n";


#  Print area header

print AREA "#AREA $area{'au'}~ $area{'ti'}~\n"
        . "$area{'ls'} $area{'us'} $area{'le'} $area{'ue'}\n\n";


#  Print recall

if (@recall) {
    print AREA "#RECALL\n";

    foreach (@recall) {
        my %recall= @{$_};
        print AREA "$recall{'rl'}\n\n";
    }
}


#  Print area special

if (@special) {
    print AREA "#AREA_SPECIAL\n";

    foreach (@special) {
        my %special= @{$_};
        if ($special{'af'})
        {
            foreach (split / /, $special{'af'}) {
                print AREA "$_\n";
            }
        }

        if ($special{'xp'})
        {
            print AREA "exp_mod $special{'xp'}\n";
        }

        if ($special{'rm'})
        {
            print AREA "reset_msg $special{'rm'}\n~\n";
        }
    }

    print AREA "\$\n\n";
}

#  Print helps

if (@helps) {
    print AREA "#HELPS";

    foreach (@helps) {
        my %help = @{$_};
        print AREA "\n$help{'lv'} $help{'he'}";
    }

    print AREA "0 \$\~\n\n";
}


#  Print mobs

if (@mobs) {
    print AREA "#MOBILES";

    foreach (@mobs) {
        my %mob = @{$_};

        print AREA "\n#" . ($mob{'vn'} + $area{'bv'}) . "\n$mob{'nm'}~\n$mob{'sh'}~\n"
                . "$mob{'lo'}\n~\n$mob{'de'}~\n$mob{'act'} $mob{'aff'} $mob{'al'} S\n"
                . "$mob{'lv'} 0 0 0d0+0 0d0+0\n$mob{'bf'} 0\n0 0 $mob{'sx'}\n";

        if ($mob{'mp'}) {
            while ($mob{'mp'} && @{$mob{'mp'}}) {
                print AREA '>', shift @{$mob{'mp'}};
            }
            print AREA "|\n";
        }

        if ( $mob{'rnk'} >= 1) {
                print AREA "< reserved~ $mob_rank[$mob{'rnk'}]\~\n";
        }

        while ($mob{'te'} && @{$mob{'te'}}) {
            print AREA shift @{$mob{'te'}};
        }
    }

    print AREA "#0\n\n";
}


#  Print objects

if (@objs) {
    print AREA "#OBJECTS";

    foreach (@objs) {
        my %obj = @{$_};

        print AREA "\n#" . ($obj{'vn'} + $area{'bv'}) . "\n$obj{'nm'}~\n"
                . "$obj{'sh'}~\n$obj{'lo'}~\n~\n$obj{'ty'} $obj{'ex'} ";

        if (&add_bits($obj{'ex'}) & $obj_ex{'trap'}) {
            print AREA "$obj{'trt'} $obj{'trd'} $obj{'trc'} ";
        }

        print AREA "$obj{'we'}\n$obj{'v0'}~ $obj{'v1'}~ $obj{'v2'}~ "
                . "$obj{'v3'}~\n$obj{'wg'} 0 $obj{'osl'}\n";

        while ($obj{'ed'} && @{$obj{'ed'}}) {
            print AREA "E\n", shift @{$obj{'ed'}};
        }

        while ($obj{'ap'} && @{$obj{'ap'}}) {
            print AREA "A\n", shift @{$obj{'ap'}};
        }

        if ($obj{'mi'}) {
            print AREA "M $obj{'mi'}\n";
        }
    }

    print AREA "#0\n\n";
}


#  Print rooms

if (@rooms) {
    print AREA "#ROOMS";

    foreach (@rooms) {
        my %room = @{$_};

        print AREA "\n#" . ($room{'vn'} + $area{'bv'})
                . "\n$room{'nm'}~\n$room{'de'}~\n0 $room{'rf'} $room{'st'}\n";

        foreach my $exit (qw/n e s w u d/) {
            if (exists $room{$exit}) {
                print AREA "D $exit_lookup{$exit}\n$room{$exit . 'de'}~\n"
                        . "$room{$exit . 'nm'}~\n$room{$exit . 'lo'} "
                        . "$room{$exit . 'ke'} " . ($room{$exit}
                        + $area{'bv'}) . "\n";
            }
        }

        while ($room{'ed'} && @{$room{'ed'}}) {
            print AREA "E\n", shift @{$room{'ed'}};
        }
        print AREA "S\n";
    }
    print AREA "#0\n\n";
}


#  Print resets

if (@resets) {
    print AREA "#RESETS\n";

    foreach (@resets) {
        &nice_tab(\$_);
        print AREA;
    }
    print AREA "S\n\n";
}

#  Print games

if (@games) {
    print AREA "#GAMES\n";
    foreach (@games) {
        &nice_tab(\$_);
        print AREA;
    }
    print AREA "S\n\n";
}


#  Print shops

if (@shops) {
    print AREA "#SHOPS\n";
    foreach (@shops) {
        &nice_tab(\$_);
        print AREA;
    }
    print AREA "0\n\n";
}


#  Print specials

if (@specials) {
    print AREA "#SPECIALS\n";
    foreach (@specials) {
        &nice_tab(\$_);
        print AREA;
    }
    print AREA "S\n\n";
}

print AREA "#\$\n";


#  All done!

if (close AREA) {
    print STDOUT "\nFinished\n";
}
else {
    print STDOUT "\nFormatted area file not closed properly\n";
}


#############################################################################
#
#  Align comments nicely
#

sub nice_tab(\$) {
    my $text = shift;
    $$text = $1 . (' ' x (32 - length $1)) . "$2\n"
            if ($$text =~ (/(.+)\t(.+)/) && length $1 < 32);
}
