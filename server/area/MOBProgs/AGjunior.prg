>greet_prog 100~
	say Welcome! I'm Kurbistraht, what can I do for you?
~
>speech_prog help quest quests quests? missions task tasks~
	if level($n) < 10
		frown $n
		mpecho 'You look a little inexperienced for what we have. Come back in a 
		mpecho few levels and see if we have something for you.'
	else 
	if level($n) > 25
		say Crom, $n, you're far too powerful for what I have.
	else
		mpecho 'What number mission are you interested in?' 
		mpecho Kurbistraht points to the large sign on his wall.
	endif
	endif
~
>speech_prog one 1 first~
	if level($n) < 10
		say I'm sorry, that one's far too dangerous for you.
	else 
	if level($n) > 25
		say I'd better let someone of lower level take that mission.
	else
		mpecho 'Two violent criminal gangs have taken control of the southeastern quarter of
		mpecho the city,' says Kurbistraht. 'All diplomatic efforts to restore order there
		mpecho have failed, and the city has decided to take stronger action. Our spies report
		mpecho that one of the gang leaders has a particularly powerful weapon he has stolen
		mpecho from a Midgaard noble. Your task is to assassinate the leader and return the
		mpecho stolen item to me.' He looks at you and grins. 'You will be well rewarded...'
	endif
	endif
~
>give_prog knuckles brass~
	if level($n) > 25
		say That mission was for someone a little less experienced.
		give brass $n
	else
	if number($o) == 2112
		say Good work, $n, you have returned the stolen item!
		mpoload 25068 10
		give egg $n
		mpjunk knuckles
		mpecho 'The egg will heal and protect you when eaten,' he says happily.
	else
		mpecho $I examines $O closely.
		say Sorry $n, I only accept the genuine article.
		give knuckles $n
	endif
~
>speech_prog 2 two second~
	if level($n) > 25
		say I'd better leave that mission for someone of higher level.
	else
	if level($n) < 15
		say You look a little inexperienced for this mission.
	else
		mpecho Kurbistraht refers to a scroll on his desk. 'A noted historian in Midgaard has
		mpecho asked the Guild to commission an adventurer to perform some field work for him.
		mpecho It is believed that the ancient hero Xerces travelled to this continent after
		mpecho the conclusion of the Serpentine War, although there is scant evidence physical
		mpecho or otherwise to confirm this. The historian believes Xerces settled in the
		mpecho Eastern Reaches, and wants you to search for evidence of his existance. He is
		mpecho prepared to exchange a substantial reward for any artefact or relic belonging
		mpecho to the hero, and offers gold for any useful information. You are to hand in any
		mpecho evidence to me and must discuss your activities or findings with no one.'
	endif
	endif
~
>speech_prog 3 three third~
	if level($n) > 25
		say I'd better let someone of lower level take that mission.
	else
	if level($n) < 15
		say You look a little inexperienced for that mission.
	else
		mpecho 'The great dragon Yevaud has been terrorising settlements on the outskirts of
		mpecho the city,' says Kurbistraht. 'Our Rangers report that many people have been
		mpecho killed or taken for food by the wyrm, who grows bolder with every raid. This
		mpecho monster must be stopped before he decides to launch a direct assault on
		mpecho Midgaard. Yevaud is often seen in flight over the forests west of the city,
		mpecho and we believe he has a lair in that area. You must destroy him and return
		mpecho any horded items to myself as evidence of his death.' Kurbistraht looks grave.
		mpecho 'Your task will be very dangerous. Good luck.'
	endif
~
>give_prog visor black knight's~
	if level($n) > 25
		say That mission was for someone a little less experienced.
		give visor $n
	else
	if number($o) == 6304
		say Great work, $n! You have protected the city.
		mpoload 25302 15
		give scroll $n
		mpjunk visor
		say Recite that scroll to increase the power of a weapon.
	else
		mpecho $I examines $O closely.
		say Sorry $n, I only accept the genuine article.
		give visor $n
	endif
~
>give_prog sword xerces~
	if level($n) > 25
		say That mission was for someone a little less experienced.
		give sword $n
	else
	if number($o) == 25243
		say Magnificent, $n, my client will be very happy!
		mpoload 25301 15
		give shield $n
		mpjunk sword
		say This shield carries many powerful enchantments.
	else
		mpecho $I examines $O closely.
		say Sorry $n, I only accept the genuine article.
		give xerces $n
	endif
~
|



