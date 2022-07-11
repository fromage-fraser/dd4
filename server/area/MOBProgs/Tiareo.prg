>greet_prog 100~
        say Welcome, I am Tiareo, the receptionist for the Reaver guildhall.  Are you interested in a 'quest'?
~
>speech_prog help quest task~
        if level($n) < 30
                frown $n
                say You are too inexperienced for us to contemplate you becoming a Reaver.  Try again when you're more experienced.
        else
        if level($n) > 40
                say $n, you're far too experienced for our current missions.  Keep checking back, though!
        else
                say Which mission are you interested in?'
                emote points to the sign on the wall.
        endif
        endif
~
>speech_prog one 1~
        if level($n) < 30
                say I'm sorry, that mission is too dangerous for someone of your limited experience.
        else
        if level($n) > 40
                say You're far too experienced to undertake that mission.
        else
                mpecho {W'The demon lord Kostchtchie has stolen a valuable hammer from Ota'a Dar's forge and fled with it to a surface realm that we have been unable to locate.'{x
                mpecho {W'We have had Reaver reports that he has been seen in the general vicinity of a dying forest, far to the east of Crystalmir Lake.'{x
				mpecho {W'Your mission is to track him down and retrieve the hammer, by fair means or foul.'{x
				emote looks at you and almost smiles
				mpecho {W'You will, of course, be compensated for your efforts.'{x
        endif
        endif
~
>speech_prog two 2~
        if level($n) < 30
                say I'm sorry, that mission is too dangerous for someone of your limited experience.
        else
        if level($n) > 40
                say You're far too experienced to undertake that mission.
        else
                mpecho {W'One of the notorious pirate lords of the Domain, Mizzenmast, has stolen the secret of sahuagin poison and heretically used it to coat one of his own weapons.'{x
				mpecho {W'Your mission is to track him down, punish him for his effrontery, and return to us with his poisoned weapon to prove that you were successful.'{x
				mpecho {W'We look forward to your triumphant and speedy return!'{x
        endif
        endif
~
>speech_prog three 3~
        if level($n) < 30
                say I'm sorry, that mission is too dangerous for someone of your limited experience.
        else
        if level($n) > 40
                say You're far too experienced to undertake that mission.
        else
                mpecho {W'The regional Reaver leader is going to visit Ota'ar Dar's branch soon.'{x
				mpecho {W'We would like to provide him with his favourite meal, which, unfortunately for us, is jellied pink skate's clasper.'{x
				mpecho {W'Pink skates are very rare in this part of the ocean, and we have had a lot of trouble trying to find one.'{x
				mpecho {W'We're not sure exactly where to look. Perhaps ask Holo Orl'as about 'skates' or 'claspers'?{x'
        endif
        endif
~
>give_prog hammer iron~
        if level($n) > 40
                say That mission was for someone a less experienced.
                give iron $n
        else
        if number($o) == 9901
				mpjunk iron
                say Good work, $n, Ahomar will be delighted!
                mpoload 27365 30
                give master $n
                say This crossbow is an excellent choice for an individual of your experience.
				mpoload 27287 30
				mpoload 27287 30
                give purple $n
                give purple $n
				say And a couple of these for the road!
        else
                mpecho $I examines $O closely.
                say Sorry $n we want the genuine hammer, not a forgery.
                give hammer $n
        endif
~

>give_prog crossbow poison~
        if level($n) > 40
                say That mission was for someone a less experienced.
                give crossbow $n
        else
        if number($o) == 17032
				mpjunk crossbow
                say Good work, $n, the secret of our poison remains secure due to your diligent effort!
                mpoload 27367 30
                give cudgel $n
				say This might be useful for having enemies take... involuntary naps?
				emote grins at you
				mpoload 27366 30
				mpoload 27327 30
				say Oh, and these might come in handy, too.
				give teal $n
				give powder $n
				
        else
                mpecho $I examines $O closely.
                say Sorry $n, I only accept the genuine article.
                give crossbow $n
        endif
~

>give_prog pink skate's clasper~
        if level($n) > 40
                say That mission was for someone a less experienced.
                give clasper $n
        else
        if number($o) == 27319
				mpjunk clasper
                say Good work, $n, the Reaver leader will be delighted!
                mpoload 27369 30
                say We had the clerics enchant these with their most powerful spells.
                give divine $n
                mpoload 27368 30
                say You should find this very useful in your travels, too.
                give holding $n
        else
                mpecho $I examines $O closely.
                say Sorry $n, this isn't quite the item we're after.
                give clasper $n
        endif
~
|
