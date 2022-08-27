>greet_prog 100~
        say Welcome, I am Tiareo, the receptionist for the Reaver guildhall.  Are you interested in a 'quest'?
~
>speech_prog help quest task~
        if level($n) < 30
                frown $n
                say You are too inexperienced for us to contemplate you becoming a Reaver.  Try again when you're more experienced.
        break
        else
        if level($n) > 40
                say $n, you're far too experienced for our current missions.  Keep checking back, though!
        break
        else
                say Which mission are you interested in?'
                emote points to the sign on the wall.
        endif
        endif
~
>speech_prog one 1~
        if level($n) < 30
                say I'm sorry, that mission is too dangerous for someone of your limited experience.
        break
        else
        if level($n) > 40
                say You're far too experienced to undertake that mission.
        break
        else
                mpecho {W'The demon lord Kostchtchie has stolen a valuable hammer from Ota'a Dar's forge'{x
                mpecho {W'and fled with it to a surface realm that we have been unable to locate. We have'{x
                mpecho {W'had Reaver reports that he has been seen in the general vicinity of a dying'{x
                mpecho {W'forest, far to the east of Crystalmir Lake. Your mission is to track him down'{x
                mpecho {W'and retrieve the hammer, by fair means or foul.'{x
                emote looks at you and almost smiles
                mpecho {W'You will, of course, be compensated for your efforts.'{x
        endif
        endif
~
>speech_prog two 2~
        if level($n) < 30
                say I'm sorry, that mission is too dangerous for someone of your limited experience.
        break
        else
        if level($n) > 40
                say You're far too experienced to undertake that mission.
        break
        else
                mpecho {W'One of the notorious pirate lords of the Domain, Mizzenmast, has stolen the'{x
                mpecho {W'secret of sahuagin poison and heretically used it to coat one of his own'{x
                mpecho {W'weapons. Your mission is to track him down, punish him for his effrontery,'{x
                mpecho {W'and return to us with his poisoned weapon to prove that you were successful.'{x
                mpecho {W'We look forward to your triumphant and speedy return!'{x
        endif
        endif
~
>speech_prog three 3~
        if level($n) < 30
                say I'm sorry, that mission is too dangerous for someone of your limited experience.
        break
        else
        if level($n) > 40
                say You're far too experienced to undertake that mission.
        break
        else
                mpecho {W'The regional Reaver leader is going to visit Ota'ar Dar's branch soon. We would'{x
                mpecho {W'like to provide him with his favourite meal, which, unfortunately for us, is'{x
                mpecho {W'jellied pink skate's clasper. Pink skates are very rare in this part of the'{x
                mpecho {W'ocean, and we have had a lot of trouble trying to find one. We're not sure'{x
                mpecho {W'exactly where to look. Perhaps ask Holo Orl'as about 'skates' or 'claspers'?{x'
        endif
        endif
~
>speech_prog four 4~
        if level($n) < 40
                say I'm sorry, that mission is too dangerous for someone of your limited experience.
        break
        else
        if level($n) > 45
                say You're far too experienced to undertake that mission.
        break
        else
                mpecho {W'A bullywug associate of ours set out some time ago to locate a froghemoth, in'{x
                mpecho {W'order to provide us with a certain part of its anatomy for use in clerical'{x
                mpecho {W'magic. He has not returned, and we must assume the worst.  Therefore we would'{x
                mpecho {W'like to charge you with his task: locate a froghemoth and bring us one of its'{x
                mpecho {W'tentacles. They are most often found in deep, muddy holes in the general area'{x
                mpecho {W'of the Ota'a Dar canal system.'{x
        endif
        endif
~
>give_prog hammer iron~
        if level($n) > 40
                say That mission was for someone a less experienced.
                give iron $n
        break
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
        break
        else
                mpecho $I examines $O closely.
                say Sorry $n we want the genuine hammer, not a forgery.
                give hammer $n
        endif
        endif
~
>give_prog crossbow poison~
        if level($n) > 40
                say That mission was for someone a less experienced.
                give poison $n
        break
        else
        if number($o) == 17032
                mpjunk poison
                mpecho {W'Good work! The secret of our poison remains secure due to your diligent effort!'{x
                mpoload 27367 30
                give cudgel $n
                mpecho {W'This might be useful for having enemies take... involuntary naps?'{x
                emote grins at you
                mpoload 27366 30
                mpoload 27327 30
                mpecho {W'Oh, and these might come in handy, too.'{x
                give powder $n
                give teal $n
        break
        else
                mpecho $I examines $O closely.
                say Sorry $n, I only accept the genuine article.
                give poison $n
        endif
        endif
~
>give_prog pink skate's clasper~
        if level($n) > 40
                say That mission was for someone a less experienced.
                give clasper $n
        break
        else
        if number($o) == 27319
                mpjunk clasper
                say Good work, $n, the leader will be delighted!
                mpoload 27369 30
                say We had our clerics enchant these with their most powerful spells.
                give divine $n
                mpoload 27368 30
                say You should find this very useful in your travels, too.
                give holding $n
        break
        else
                mpecho $I examines $O closely.
                say Sorry $n, this isn't quite the item we're after.
                give clasper $n
        endif
        endif
~
>give_prog froghemoth tentacle~
        if level($n) > 45
                say That mission was for someone less experienced.
                give tentacle $n
        break
        else
        if number($o) == 27412
                mpjunk tentacle
                say Great work, $n, the clerics will be overjoyed!
                mpoload 27413 40
                mpoload 27413 40
                say We had the clerics whip you up something special by way of reward.
                give vial $n
                give vial $n
        break
        else
                mpecho $I examines $O closely.
                say Sorry $n, this isn't quite the item we're after.
                give clasper $n
        endif
        endif
~
|
