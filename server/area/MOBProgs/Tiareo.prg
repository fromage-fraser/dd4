>greet_prog 100~
        mpecho {WReaver Tiareo says 'Welcome, I am Tiareo, the receptionist for the Reaver guildhall.  Are you interested in a 'quest'?'{x
~
>speech_prog help quest task~
        if level($n) < 30
                frown $n
                mpecho {WReaver Tiareo says 'You are too inexperienced for us to contemplate you becoming a Reaver.  Try again when you're more experienced.'{x
        break
        else
        if level($n) > 70
                mpecho {WReaver Tiareo says '$n, you're far too experienced for our current missions.  Keep checking back, though!'{x
        break
        else
                mpecho {WReaver Tiareo says 'Which mission are you interested in?'{x
                emote points to the sign on the wall.
        endif
        endif
~
>speech_prog one 1~
        if level($n) < 30
                mpecho {WReaver Tiareo says 'I'm sorry, that mission is too dangerous for someone of your limited experience.'{x
        break
        else
        if level($n) > 40
                mpecho {WReaver Tiareo says 'You're far too experienced to undertake that mission.'{x
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
                mpecho {WReaver Tiareo says 'I'm sorry, that mission is too dangerous for someone of your limited experience.'{x
        break
        else
        if level($n) > 40
                mpecho {WReaver Tiareo says 'You're far too experienced to undertake that mission.'{x
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
                mpecho {WReaver Tiareo says 'I'm sorry, that mission is too dangerous for someone of your limited experience.'{x
        break
        else
        if level($n) > 40
                mpecho {WReaver Tiareo says 'You're far too experienced to undertake that mission.'{x
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
                mpecho {WReaver Tiareo says 'I'm sorry, that mission is too dangerous for someone of your limited experience.'{x
        break
        else
        if level($n) > 45
                mpecho {WReaver Tiareo says 'You're far too experienced to undertake that mission.'{x
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
>speech_prog five 5~
        if level($n) < 40
                mpecho {WReaver Tiareo says 'I'm sorry, that mission is too dangerous for someone of your limited experience.'{x
        break
        else
        if level($n) > 45
                mpecho {WReaver Tiareo says 'You're far too experienced to undertake that mission.'{x
        break
        else
                mpecho {W'We have received a letter from one of the residents of the Vale who wishes us'{x
                mpecho {W'to locate a servant of his--a gypsy man--who was last seen in the vicinity of'{x
                mpecho {W'the Yeats Sea. As he is unable to 'sense his presence' but also knows he is'{x
                mpecho {W'not dead, he believes him to have been abducted by the sahuagin. Prisoners'{x
                mpecho {W'are usually taken to the arena, so I would try there first. Here is the letter.'{x
                mpoload 27504
                give letter $n
        endif
        endif
~
>speech_prog six 6~
        if level($n) < 45
                mpecho {WReaver Tiareo says 'I'm sorry, that mission is too dangerous for someone of your limited experience.'{x
        break
        else
        if level($n) > 50
                mpecho {WReaver Tiareo says 'You're far too experienced to undertake that mission.'{x
        break
        else
                mpecho {W'One of the elder Reavers is near death, and his dying wish is that we serve'{x
                mpecho {W'him a plate of seahorse scramble 'just like his mother used to make'. The dish'{x
                mpecho {W'he describes we believe only to be made by one of the army cooks in the military'{x
                mpecho {W'canton. Please track it down and bring it here; we will make it worth your while.'{x
        endif
        endif
~
>speech_prog seven 7~
        if level($n) < 50
                mpecho {WReaver Tiareo says 'I'm sorry, that mission is too dangerous for someone of your limited experience.'{x
        break
        else
        if level($n) > 55
                mpecho {WReaver Tiareo says 'You're far too experienced to undertake that mission.'{x
        break
        else
                mpecho {W'We have a request from Mohmit for the eyes of a rare and elusive deep sea fish,'{x
                mpecho {W'for use in one of his magical concoctions. Usefully, he hasn't given us any'{x
                mpecho {W'information about it, so you may wish to go and ask him for more details before'{x
                mpecho {W'you go looking. Bring the eyes back here for us to inspect before we pass them on,'{x
                mpecho {W'and you shall be well-rewarded.'{x
        endif
        endif
~
>speech_prog eight 8~
        if level($n) < 50
                mpecho {WReaver Tiareo says 'I'm sorry, that mission is too dangerous for someone of your limited experience.'{x
        break
        else
        if level($n) > 70
                mpecho {WReaver Tiareo says 'You're far too experienced to undertake that mission.'{x
        break
        else
                mpecho {W'There's nothing Reaver Maeril loves more than a trip to the surface (or the'{x
                mpecho {W'slave market) for a relaxing smoke, but he's inordinately fussy about what'{x
                mpecho {W'he'll consume.  He's a big fan of spongeweed, but it's become almost impossible'{x
                mpecho {W'to get hold of.  The rumour is that someone in the Prince's palace has a supply,'{x
                mpecho {W'but it'll be no easy task to get in there and acquire it.  Maeril has given me'{x
                mpecho {W'permission to reward you very handsomely if you manage to, though.'{x
        endif
        endif
~
>give_prog spongeweed tobacco~
        if level($n) > 70
                mpecho {WReaver Tiareo says 'That mission was for someone less experienced.'{x
                give spongeweed $n
        break
        else
        if number($o) == 27606
                mpjunk spongeweed
                mpecho {WReaver Tiareo says 'Excellent $n, Maeril will be delighted!'{x
                mpoload 27627 70
                give magical $n
                mpecho {WReaver Tiareo says 'Have this magical bag of holding; it's worth a fortune!'{x
        break
        else
                mpecho $I examines $O closely.
                mpecho {WReaver Tiareo says 'It's not the genuine article.  Sorry $n.'{x
                give spongeweed $n
        endif
        endif
~
>give_prog hideous upturned eyes~
        if level($n) > 55
                mpecho {WReaver Tiareo says 'That mission was for someone less experienced.'{x
                give hideous $n
        break
        else
        if number($o) == 27566
                mpjunk hideous
                mpecho {WReaver Tiareo says 'Excellent $n, Mohmit will reward us richly!'{x
                mpoload 27567 50
                give silver $n
                mpecho {WReaver Tiareo says 'Take this magically-enhanced beltpouch as a token of our gratitude.'{x
        break
        else
                mpecho $I examines $O closely.
                mpecho {WReaver Tiareo says 'Wrong eyes.  Sorry $n.'{x
                give hideous $n
        endif
        endif
~
>give_prog seahorse scramble plate~
        if level($n) > 50
                mpecho {WReaver Tiareo says 'That mission was for someone less experienced.'{x
                give seahorse $n
        break
        else
        if number($o) == 27522
                mpjunk seahorse
                mpecho {WReaver Tiareo says 'Excellent $n, the elder will die happy!'{x
                mpoload 27523 45
                give starry $n
                mpecho {WReaver Tiareo says 'These leggings will bolster your endurance and magical power.'{x
        break
        else
                mpecho $I examines $O closely.
                mpecho {WReaver Tiareo says 'Sorry $n we want the real dish, not some amateur approximation.'{x
                give seahorse $n
        endif
        endif
~
>give_prog hammer iron~
        if level($n) > 40
                mpecho {WReaver Tiareo says 'That mission was for someone less experienced.'{x
                give iron $n
        break
        else
        if number($o) == 9901
                mpjunk iron
                mpecho {WReaver Tiareo says 'Good work, $n, Ahomar will be delighted!'{x
                mpoload 27365 30
                give master $n
                mpecho {WReaver Tiareo says 'This crossbow is an excellent choice for an individual of your experience.'{x
                mpoload 27287 30
                mpoload 27287 30
                give purple $n
                give purple $n
                mpecho {WReaver Tiareo says 'And a couple of these for the road!'{x
        break
        else
                mpecho $I examines $O closely.
                mpecho {WReaver Tiareo says 'Sorry $n we want the genuine hammer, not a forgery.'{x
                give hammer $n
        endif
        endif
~
>give_prog crossbow poison~
        if level($n) > 40
                mpecho {WReaver Tiareo says 'That mission was for someone less experienced.'{x
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
                mpecho {WReaver Tiareo says 'Sorry $n, I only accept the genuine article.'{x
                give poison $n
        endif
        endif
~
>give_prog pink skate's clasper~
        if level($n) > 40
                mpecho {WReaver Tiareo says 'That mission was for someone less experienced.'{x
                give clasper $n
        break
        else
        if number($o) == 27319
                mpjunk clasper
                mpecho {WReaver Tiareo says 'Good work, $n, the leader will be delighted!'{x
                mpoload 27369 30
                mpecho {WReaver Tiareo says 'We had our clerics enchant these with their most powerful spells.'{x
                give divine $n
                mpoload 27368 30
                mpecho {WReaver Tiareo says 'You should find this very useful in your travels, too.'{x
                give holding $n
        break
        else
                mpecho $I examines $O closely.
                mpecho {WReaver Tiareo says 'Sorry $n, this isn't quite the item we're after.'{x
                give clasper $n
        endif
        endif
~
>give_prog froghemoth tentacle~
        if level($n) > 45
                mpecho {WReaver Tiareo says 'That mission was for someone less experienced.'{x
                give tentacle $n
        break
        else
        if number($o) == 27412
                mpjunk tentacle
                mpecho {WReaver Tiareo says 'Great work, $n, the clerics will be overjoyed!'{x
                mpoload 27413 40
                mpoload 27413 40
                mpecho {WReaver Tiareo says 'We had the clerics whip you up something special by way of reward.'{x
                give vial $n
                give vial $n
        break
        else
                mpecho $I examines $O closely.
                mpecho {WReaver Tiareo says 'Sorry $n, this isn't quite the item we're after.'{x
                give clasper $n
        endif
        endif
~
>give_prog letter shaky~
        if level($n) > 45
                mpecho {WReaver Tiareo says 'That mission was for someone less experienced.'{x
                give shaky $n
        break
        else
        if number($o) == 27505
                mpjunk shaky
                mpecho {WReaver Tiareo says 'Excellent, $n, I will ensure this letter finds its addressee.'{x
                mpoload 27506 40
                mpecho {WReaver Tiareo says 'Have one of these excellent stunning weapons by way of reward.'{x
                give ironwood $n
        break
        else
                mpecho $I examines $O closely.
                mpecho {WReaver Tiareo says 'Sorry $n, this isn't an authentic item.'{x
                give shaky $n
        endif
        endif
~
|
