>greet_prog 100~	
	mpecho 'Welcome to the Adventurer's Guild,' says the Secretary. 
	mpecho She smiles warmly. 'What can I do for you?' ~

>speech_prog help quests quest task how where quest? task? mission~
	look $n
	if level($n) < 10
		mpecho 'You look a little inexperienced,' the Secretary notes. 
		mpecho 'Come back when you're a little stronger.'
	else 
	if level($n) < 26
		say I suggest you see Lord Kurbistraht to the north
	else
	if level($n) < 46
		say Saint Justinian will assist you, $n.
		mpecho She points to the room directly east from here.
	else
		mpecho 'Xiao Tsu handles our most experienced adventurers.
		mpecho Her office is south from here.'
	endif
        endif
        endif
~
|

		
