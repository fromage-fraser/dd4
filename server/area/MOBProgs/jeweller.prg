>speech_prog quest?  quest~
if level($n) < 15
mpecho {W The jeweller says 'I have two tasks for someone of your
mpecho {W calibre. The first is to try to locate a small diamond
mpecho {W that I lost while visiting the circus outside Midgaard.
mpecho {W I've heard rumours that the circus owner may have tried
mpecho {W to sell a diamond similar to the one I'm missing.
mpecho {W{x
mpecho {W The second task is a little more dangerous. I have heard
mpecho {W that there are sapphires beneath the city. In the dark
mpecho {W alley, a grate leads down to a rat infested lair. In said
mpecho {W lair, jewels are plentiful. Bring me a sapphire. I will
mpecho {W reward you well...'{x
    break
else
 if level($n) < 20
        think
mpecho {W The jeweller says 'I cannot think of any errands that
mpecho {W need doing.  Perhaps the Adventurers' Guild can help you.'{x
    break
else
 if level($n) < 30
mpecho {W The jeweller says 'Far to the east in a mighty desert
mpecho {W there is a dragons lair. Deep within the lair many
mpecho {W jewels lie. Find me a lapis gem. I will reward you
mpecho {W well for it..'{x
endif
mpecho {W The jeweller says 'I have no quests for someone of your
mpecho {W calibre.  Perhaps the Adventurers' Guild can help you.'{x
endif
endif
~
>greet_prog 100~
 if ispc($n)
        say Welcome to my humble shop. Are you interested in a 'quest'?
 endif
~
> give_prog sapphire gem~
    if level($n) > 15
        say That mission was for someone a little less experienced.
        give $o $n
        break
    endif

    if number($o) == 3822
            say Well done $n, here is your reward.
            mpoload 3382 1
            give bracer $n
            mpjunk sapphire
            break
    else
            mpecho $I examines $O closely.
            say I only accept the genuine article, take it away $n!
            give $o $n
    endif
~
> give_prog gem lapis~
    if level($n) > 30
        say That mission was for someone a little less experienced.
        give $o $n
        break
    endif

    if number($o) == 5029
            say Great work $n, thanks a lot.
            mpoload 3383 20
            say Please accept this token of my appreciation.
            give sceptre $n
            mpjunk gem
            break
    else
            mpecho $I examines $O closely.
            say Sorry $n, I only accept the genuine article.
            give $o $n
    endif
~
> give_prog small diamond~
    if level($n) > 15
        say That mission was for someone a little less experienced.
        give $o $n
        break
    endif

    if number($o) == 4451
            say Good work, $n, you have returned my missing diamond!!
            mpoload 10311 1
            give talisman $n
            mpjunk diamond
            mpecho {W'This magic talisman will increase your wisdom' he says happily.{x
            break
    else
            mpecho $I examines $O closely.
            say I'm sorry $n, but this isn't my diamond.
            give $o $n
    endif
~
|
