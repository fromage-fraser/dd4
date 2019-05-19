>entry_prog 30~
emote keeps an eye out for wanted criminals.
~
>greet_prog 40~
look $n
if isgood($n)
	smile $n
	say I hear the jeweller needs a task performed.
else
	mpechoaround $n $I wonders if $n is among the most wanted.
	mpechoat $n $I wonders if you are among the most wanted.
endif
~
>rand_prog 20~
if rand(50)
	emote whistles a little song.
else
	sing
endif
~
>act_prog p bursts into~
say What is wrong?
~
>speech_prog p My girlfriend left me~
comfort $n 
~
|

