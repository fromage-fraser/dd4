>greet_prog 100~
        say Sit down. What do you want? I am very busy at the moment.
~
>speech_prog help quest quests quests? task tasks~
        if level($n) < 55
            frown $n
            mpecho 'You do not have the experience to embark on the dangerous missions I have.
            mpecho Go away and stop wasting my time.'
        else
        if level($n) > 65
            hmm $n
            mpecho 'I have no missions for someone of your calibre and experience. Go away.'
        else
            mpecho 'I may have something for you to do, check the board.'
            mpecho Xiao Tsu jerks her thumb towards the large sign behind her.
        endif
        endif
~
>speech_prog one 1 first~
          if level ($n) < 55
                say Sorry $n, that mission requires someone less useless and puny than yourself.
          else
          if level ($n) > 65
                say That mission is for someone less experienced than yourself, $n. Stop wasting my time.
          else
                mpecho 'I have been contacted by a famous magician from Draagdim who is trying to
                mpecho assemble an ancient jewelled stave, believed to have powerful magic properties.
                mpecho Unfortunately the source of the artifact's power, a large enchanted ruby, was
                mpecho long ago prised from the rod and stolen by persons unknown. For many years the
                mpecho sorcerer has looked for the gem in vain, though he believes the thief lurks
                mpecho somewhere deep in Baron Sumkumvit's dungeon.'
                mpecho {x
                mpecho Rumour has reached us that the villain responsible for the theft is none other
                mpecho than one of the Dungeon's notorious leprechauns. We think it unlikely he would
                mpecho conceal the gem on his person, though doubt that he would hide it far from his
                mpecho hunting grounds, for reasons of convenience and easy access in an emergency.
                mpecho {x
                mpecho The magician has offered a powerful protective talisman as a reward for the
                mpecho stone, should you manage to effect its safe return.'
          endif
          endif
~
>give_prog ruby~
        if level($n) < 55
                say Sorry $n, but you don't qualify for that mission.
                give ruby $n
        break
        else
                if number($o) == 17832
                say Well done $n, our magician friend will be very pleased indeed.
                mpoload 25308 55
                give talisman $n
                mpjunk ruby
        else
                mpecho $I examines $O closely.
                say Sorry $n, this is not the gem we're looking for.
                give ruby $n
        endif
        endif
~
>speech_prog p load tear~
        if name($n) == geshp
                mpoload 25250 1
                drop tear
        else
        endif
~
|

