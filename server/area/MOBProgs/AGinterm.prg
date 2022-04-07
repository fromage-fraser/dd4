>greet_prog 100~
        say Greetings. I am Justinian. What may I do for you?'
~
>speech_prog help quest quests quests? task tasks~
        if level($n) < 25
            frown $n
            mpecho 'You do not have the experience to embark on the dangerous missions I have.
            mpecho Go see Lord Kurbistraht, or otherwise come back in a few levels and
            mpecho I'll reassess my decision.'
        else
        if level($n) > 60
            hmm $n
            mpecho 'I fear you would be wasting your precious time on my easy missions.'
        else
            smile
            mpecho 'I'm sure I can assist you, what mission are you interested in?'
            mpecho St. Justinian gestures to a large SIGN.
        endif
        endif
~
>speech_prog one 1 first~
          if level ($n) < 25
                say I'm sorry $n, that one's far too dangerous for you.
          else
          if level ($n) > 45
                say $n, I'd better let someone of lower level do that mission.
          else
                mpecho 'In ancient times the city of Solace was the frequent victim of attack by
                mpecho dragons. In recent years dragons have mysteriously abandoned their attacks on
                mpecho this city: for some reason dragon's avoid Solace, and we fear Midgaard may be
                mpecho targeted in its place. Intelligence reports tell us that the Solace guard
                mpecho have a special weapon that has been the bane of many dragons. Your mission is
                mpecho to find this weapon and deliver it to me.'
          endif
          endif
~
>speech_prog 2 2! 2? two two? two! second second? second!~
          if level ($n) < 25
                say I'm sorry $n, that one's far too dangerous for you.
          else
          if level ($n) > 40
                say $n, I'd better let someone of lower level do that mission.
          else
                mpecho 'Midgaard and the Good Cities have traditionally kept a strong alliance with
                mpecho the Elven settlements on the continent,' begins Justinian. 'I have received
                mpecho several communications from the Silvan Elves settled north of the city. They
                mpecho have suffered constant harrassment from Drow raiders that plunder their lush
                mpecho valleys: they ask that Midgaard provide assistance in their efforts to curb
                mpecho Dark Elf surface activity. We are bound by treaty to help them, and the timing
                mpecho for such action is good for our ends, as the Drow are a plague that must be
                mpecho stamped out. A large Drow complex is hidden underground to the east of the
                mpecho city. Penetrate the city and assassinate the leader, who is reputed to have
                mpecho strong magical ability. Bring me any items as evidence of his death; with the
                mpecho leader dead, our forces can massacre the horde before they have a chance to
                mpecho regroup. Be careful, as the Drow are highly aggressive and dangerous.'
        endif
        endif
~
>speech_prog 3 3! 3? three three! third third! third?~
        if level($n) > 50
                say I'd better let someone of lower level take that mission.
        else
        if level($n) < 35
                say You look a little inexperienced for that mission.
        else
                mpecho 'A powerful magical artifact was stolen some weeks ago from the City
                mpecho Treasury and all our attempts to recover it have failed.'
                mpecho {x
                mpecho 'Rumour has it a powerful being who resides in the Warp at the top
                mpecho of the Ultima kingdom may be responsible for the theft. You are charged
                mpecho with the mission of investigating this crime and recovering the item.
                mpecho The City authorizes you to use lethal force if necessary.'
        endif
        endif
~
>speech_prog 4 4! 4? four four! four? fourth fourth! fourth?~
        if level($n) < 45
                say I'm sorry $n, that one's far too dangerous for you.
        else
        if level ($n) > 60
                say $n, I'd better let someone of lower level do that mission.
        else
                mpecho 'You are charged with the recovery of a powerful magical item looted from the
                mpecho corpse of a high-ranking Ofcol Dragonknight, slain in cold blood by Draconian
                mpecho marauders from the west,' says Justinian. 'The Chromatic Dragon Orb is thought
                mpecho to be in the possession of Tiamat, a gigantic dragon that rules over a horde
                mpecho of serpents based in a tower in the very heart of the western forest. Bring me
                mpecho the orb and be well rewarded.'
        endif
        endif
~
>give_prog sword dragonslicer~
        if level($n) > 45
        say That mission was for someone a little less experienced.
        give dragonslicer $n
        else
                if level($n) < 25
                say Sorry $n, but you don't qualify for that mission.
                give dragonslicer $n
        break
        else
                if number($o) == 10215
                say Well done $n, the city will now be safe.
                mpoload 25306 25
                give vest $n
                mpjunk sword
        else
                mpecho $I examines $O closely.
                say Sorry $n, I only accept the genuine article.
                give dragonslicer $n
        endif
        endif
~
>give_prog dagger sacrificial~
        if level($n) > 45
                say That mission was for someone a little less experienced.
                give dagger $n
        else
        if level($n) < 25
                say Sorry $n, you don't qualify for that mission.
                give dagger $n
        else
        if number($o) == 5113
                say Well done $n, you have destroyed the Drow prince!
                mpoload 25250 25
                give tear $n
                say Brandishing the tear will lend you the gods' protection.
                mpjunk dagger
        else
                mpecho $I examines $O closely.
                say Sorry $n, I only accept the genuine article.
                give sacrificial $n
        endif
~
>give_prog scepter power artifact~
        if level($n) > 50
                say That mission was for someone a little less experienced.
                give scepter $n
        else
        if level($n) < 35
                say Sorry $n, you don't qualify for that mission.
                give scepter $n
        else
        if number($o) == 2469
                say $n, you have recovered the item! Congratulations!
                say The City thanks you for your brave service.
                mpoload 25307 35
                give mithril $n
                mpoload 25307 35
                give mithril $n
                say May these chokers serve you well in combat.
                mpjunk scepter
        else
                mpecho $I examines $O closely.
                say Sorry $n, this isn't the artifact we're looking for.
                give scepter $n
        endif
~
>give_prog orb chromatic dragon~
        if level($n) > 60
                say That mission was for someone a little less experienced.
                give orb $n
        else
        if level($n) < 50
                say Sorry $n, you don't qualify for that mission.
                give orb $n
        else
        if number($o) == 2298
                say $n, you have recovered the Orb, well done!
                mpoload 25305 50
                give emerald $n
                say The necklace will increase your battle prowess.
                mpjunk orb
        else
                mpecho $I examines $O closely.
                say Sorry $n, I only accept the genuine article.
                give chromatic $n
        endif
~
|


