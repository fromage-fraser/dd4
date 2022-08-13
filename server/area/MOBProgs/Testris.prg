>greet_prog 35~
if rand(25)
	whistle $n
	say See anything you... like?
	snicker
else
	if rand (25)
		wink $n
	else
		if rand(25)
			lick
		else
			say For the right amount of copper I can provide... "demonstrations"?  See our sign for prices.
			grin
		endif
	endif
endif
~
>bribe_prog 1000~
	wake
	mpjunk coins
	say Ah, the crown jewel of my art! Let's go!
	mpecho {wTestris gleefully takes you up to the training dungeon.{x
	mptransfer $n 27646
	mpgoto 27646
	mpforce $n look
	mpecho {wTestris stomps on the floor and his long-suffering assistant, Sogesk, traipses wearily into the room.{x
	say Today I shall demonstrate my absolutely favourite the device - the rack!
	mpecho Sogesk regards his battered body with weary resignation.
	say This rack consists of a rectangular wooden frame, raised from the ground, with a roller at both ends. The victim's ankles are fastened to one roller and their wrists chained to the other.
	say When used as a torture device in its own right, the handle and ratchet mechanism are used to gradually retract the chains, slowly increasing the strain on the victim's shoulders, hips, knees, and elbows and causing excruciating pain.
	say If muscle fibres are stretched excessively, they will lose their ability to contract, rendering them permanently useless.
	say By means of pulleys and levers, the roller can also be rotated on its own axis, thus straining the ropes until the victim's joints are dislocated and eventually completely separate. This rack can also be rotated vertically, so the torturer can work on his victim face-to-face, as it were.
	say On you hop, Sogesk.
	mpecho {wSogesk reluctantly lies down on the rack and Testris chains his legs and arms to it.{x
	say One particularly enjoyable aspect of stretching victims on the rack is the loud popping noises made by snapping cartilage, ligaments and bones. Confinement on the rack will also immobilise victims in a way that enables further tortures to be easily and simultaneously applied to them.
	mpecho {wSogesk's breathing becomes increasingly shallow.{x
	say Well Sogesk, delay is the enemy of progress! Let's crack on!
	emote giggles
	say 'Crack' on!  I see what I did there!
	mpecho {wSogesk seems considerably less amused by the quip than Testris does.{x
	mpecho {wTestris grabs the rack's handle and starts to rotate it around.{x
	mpecho Sogesk makes no sound for a little while, then starts to grunt.
	say Feeling the 'burn,' Sogesk?  It begins!
	mpecho {R::POP::{x
	mpecho {wSogesk shrieks.{x
	emote giggles like a schoolgirl
	mpecho {R::SNAP::{x
	mpecho {wOne of the ligaments in Sogesk's left arm snaps in half.  The consequent sound from his mouth is... indescribable.{x
	emote snorts with pleasure, turning the handle even more enthusiastically
	mpecho {R::BANG:: ::BANG::{x
	mpecho {wBoth of Sogesk's legs are torn from their sockets, coming completely off.  Arterial blood gushes from the obliterated stumps where the legs once connected.  After a bloodcurdling shriek dying away to a soft moan Sogesk falls completely silent, dead.{x
	say Oh dear, perhaps I was a little... enthusiastic.  Resurrection spells are AWFULLY expensive.
	mpecho Both of you regard Sogesk's lifeless, mutilated body with consternation, though perhaps for different reasons.
	say Well, my fault.  I'll get him to Nirlisk shortly; hopefully there's not too much of a backlog.
	emote hmms
	say Please, come this way.
	mpecho {wTestris leaves Sogesk's corpse to bleed out and the two of you return to the shop.{x
	mptransfer $n 27346
	mpgoto 27346
	mpforce $n look
	mpoload 27309 30
	say Obviously I can't give you a rack to take away, but perhaps you'll find this item... useful.
	give slimy $n
	wink $n
	say At your command, $n. As ever.
~
>bribe_prog 750~
	wake
	mpjunk coins
	say Hard to go wrong with a rod!  After me, if you please.
	mpecho {wTestris leads you up to the training dungeon, on the second level of the building.{x
	mptransfer $n 27646
	mpgoto 27646
	mpforce $n look
	mpecho {wTestris shouts, 'Sogesk!' and his so-named assistant limps into the room.{x
	say Today we will demonstrate a birch rod, often just referred to as a 'birch'.
	mpecho Sogesk slumps.  You can tell his spirit was broken completely a long time ago.
	say Rack again, Sogesk?
	mpecho Sogesk sighs and nods.
	mpecho {wTestris has Sogesk lie down on the rack and chains his legs and arms to it, rendering him immobile.  He then pulls a lever at the side to tilt the rack vertical.{x
	say Contrary to what its name would suggest, the birch rod is not a single rod, but a bundle of leafless twigs bound together.  It may also be made of other kinds of branches, so long as they are strong and smooth.  Willow and hazel are comparably popular. Poplar, however, isn't.
	emote laughs uproariously at his pun.
	emote wipes a tear from his eye and clears his throat.
	say Ahem.  Excuse me.  What's life without a few laughs, I always say?
	mpecho {wTestris grabs the birch rod from its resting place in the corner of the room.{x
	mpecho Sogesk's body shakes a little.
	emote smiles in a way that is suggestive of vulgarity
	say A prime factor in the severity of damage a birch rod inflicts is its size: that is to say its length, weight and the number of branches included in the bundle.  The ones we sell tend to err on the side of substantiality.
	mpecho {wTestris swishes the birch rod through the water a few more times.{x
	say Looks like I'm running out of reasons not to hurt you, doesn't it Sogesk?
	mpecho Sogesk sighs deeply, bowing his head.
	mpecho {wTestris cricks his neck and stretches his arms.{x
	mpecho {wTestris's whip {x{r***{x {RDEVASTATES{x {r***{x {wSogesk!{x
	mpecho {wTestris's whip {x{r***{x {RDEVASTATES{x {r***{x {wSogesk!{x
	mpecho {wTestris's whip <0><124>***<0> <196>ANNIHILATES<0> <124>***<0> {wSogesk!{x
	mpecho {wTestris's whip <0><124>-=<<**<160><558>OBLITERATES<0><124>**>=-<0> {wSogesk!{x
	mpecho Sogesk cries out with every blow.
	emote bends over, breathing heavily
	say Thirsty work!  It can help to have a few people working in rotation to deliver the beating.
	mpecho Sogesk sobs and shakes.
	say Cheer up lad, at least you're not a surface-dweller! Err.. no offense of course, $n.
	emote clears his throat, a little awkwardly
	say If you'll follow me again.
	mpecho {wTestris leaves Sogesk to recover and returns you to the shop's lower level.{x
	mptransfer $n 27346
	mpgoto 27346
	mpforce $n look
	mpoload 27361 30
	say Thrash your enemies with absolute assurance of quality!
	give birch $n
	say Your humble servant, as always.
~
>bribe_prog 725~
	wake
	mpjunk coins
	say A popular choice.  If you please?
	mpecho {wYou trail behind Testris as he swims up to the second level of the shop.{x
	mptransfer $n 27646
	mpgoto 27646
	mpforce $n look
	mpecho {wTestris calls for his assistant, Sogesk, who slopes into the room a few moments later.{x
	say Today we will demonstrate the use of a batog, a kind of rod or stick optimised for the administration of prolonged beatings.
	mpecho Sogesk's shakes his head sadly.
	say Rack again for this one I think, Sogesk.
	mpecho Sogesk nods even more sadly.
	mpecho {wTestris has Sogesk lie on the rack face-down and chains his legs and arms to it, exposing his back.  He collects a batog from its holster near the rack, swishing it through the water a few times experimentally.{x
	say The batog is often used by those in our profession to administer corporal punishment intended to be severe but usually not fatal. Emphasis on the USUALLY.
	mpecho Sogesk is silent, you see him brace his body for what he knows is coming.
	emote smacks his lips in anticipation
	say Oh, and I should mention that we like to enchant our batogs here with clerical 'harm' spells for a little extra spice.
	mpecho {wTestris zaps Sogesk with a batog.{x
	mpecho {wTestris's mindpower decimates Sogesk!
	mpecho {wTestris zaps Sogesk with a batog.{x
	mpecho {wTestris's mindpower mangles Sogesk!
	mpecho {wTestris zaps Sogesk with a batog.{x
	mpecho {wTestris's mindpower mauls Sogesk!
	mpecho {wTestris zaps Sogesk with a batog.{x
	mpecho {wTestris's mindpower decimates Sogesk!
	mpecho {wTestris zaps Sogesk with a batog.{x
	mpecho {wTestris's mindpower decimates Sogesk!
	emote huffs and puffs
	say It's quite a tiring business.  Ideally use 2-3 people to do the beating and rotate them when they get tired.
	mpecho You can't see Sogesk's face, but his body shudders involuntarily, and he gasps and groans loudly.
	say Still, I daresay you get the idea!
	snicker
	say A pleasure as always, $n
	mpecho {wTestris replaces the batog in its holster and guides you back down to the shop.{x
	mptransfer $n 27346
	mpgoto 27346
	mpforce $n look
	mpoload 27361 30
	say To be used in sickness and health.
	give batog $n
	say See you again soon, $n.
~
>bribe_prog 700~
	wake
	mpjunk coins
	say Excellent.  Please, follow me.
	mpecho {wYou follow Testris up to the second level of the building.{x
	mptransfer $n 27646
	mpgoto 27646
	mpforce $n look
	mpecho {wTestris coughs loudly and a couple of minutes later his assistant, Sogesk, gloomily swims into the room.{x
	say Today we will demonstrate a breast ripper, also known as an 'iron spider,' or, simply a 'spider'.
	mpecho Sogesk's mouth trembles nervously.
	say I think you might best be accomodated by the rack for this demonstration, Sogesk, don't you?
	mpecho Sogesk sighs dejectedly.
	mpecho {wTestris has Sogesk lie down on the rack and chains his legs and arms to it, rendering him immobile, spreadeagled, and horizontal.{x
	mpecho Sogesk stares at the ceiling.  If he has a soul, it has left his body.{x
	say The breast ripper has historically been used on women, but we'll make an exception today for Sogesk here.  It is usually heated before application.
	mpecho {wTestris places the ripper in a small brazier next to the rack, which burns with a magical heat source.{x
	mpecho Sogesk is silent, his face completely vacant.
	emote quivers with excitement
	say The breast ripper consists of four 'claws,' which are used to slowly rip the breasts off individuals, one at a time. They are designed chiefly to produce a 'shredding,' or 'tearing' effect as they remove the breasts of the victim.
	mpecho {wTestris removes the ripper from the brazier with a large pair of tongs.  It glows almost white with heat.{x
	say Never a dull moment, eh Sogesk?
	mpecho {wTestris manoeuvres the ripper towards Sogesk's chest, then pushes it against his skin. Sogesk screams. You hear a loud sizzling sound and the water fills with the pungent stench of burning sahuagin.  The sizzling eventually stops.  The screams do not.{x
	mpecho {wTestris's face is a picture of delight.{x
	say And now we use the tongs to close the ripper, thereby removing the breast.  This is best done in fits and starts, to maximise the victim's suffering, and get the best bang for your coppers.
	mpecho Sogesk's screams rise in pitch and intensity as, over the next few minutes, his left breast is removed by the device with what might be euphemistically described as 'unimaginable cruelty'.
	mpecho {wThe lifeless, charred breast floats to the ground. Sogesk continues to scream. {x
	say Now, now Sogesk.  Another day, another copper, yes?
	mpecho {wTestris's eyes dilate slightly, and he emits what you interpret as a quiet moan of ecstasy.{x
	mpecho Sogesk continues to scream.  You don't imagine he'll stop any time soon.
	say What a drama queen. Silly old duck.
	cough
	say Another day, another successful demonstration for a valued customer.
	mpecho {wTestris leaves Sogesk to scream and takes you back down to the shop's lower level.{x
	mptransfer $n 27346
	mpgoto 27346
	mpforce $n look
	mpoload 27360 30
	say I hope you enjoy it as much as I have.
	give ripper $n
	say Please come back soon!
~
>bribe_prog 675~
	wake
	mpjunk coins
	say Say no more!  Right this way.
	mpecho {wYou follow Testris up to the second level of the building.{x
	mptransfer $n 27646
	mpgoto 27646
	mpforce $n look
	mpecho {wTestris claps his hands together and his assistant Sogesk trudges wearily into the room.{x
	say Today we will show the correct operation of a pear of anguish, variously known as a choke pear or mouth pear.
	mpecho Sogesk eyes widen.  He silently mouths the word 'NO'.
	mpecho {wTestris straps Sogesk into the large wooden chair, and fetches what looks like a large mechanical pear with a key in it from a shelf.{x
	mpecho Sogesk shakes his head, trembling with terror.{x
	say The mechanism of a pear of anguish consists of a pear-shaped metal body divided into spoon-like segments that can be spread apart by turning a key. It may be variously inserted into the mouth, rectum, or vagina of its victim, and can then be expanded to gag or (ideally!) to mutilate them.
	say Which orifice is getting lucky today, Sogesk?
	leer
	mpecho Sogesk squeaks, then clamps his mouth shut.
	say That which told it, holds it.  Open wide!
	mpecho {wTestris pushes the pear into Sogesk's mouth.  As he forces it in deeper Sogesk's protestations become muffled, then cease entirely.{x
	say The screw that holds the pear 'segments' closed will slowly release pressure on them as we turn this key to rotate its thread.{x
	mpecho {wSogesk eyes become glassy and his pupils contract to pinpoints.{x
	lick
	say The fun never ends, does it, Sogesk?
	mpecho {wTestris starts to turn the key.  As he does, the shape of the segments becomes increasingly outlined on Sogesk's bulging cheeks.{x
	mpecho {wAfter several minutes Sogesk grunts thickly, and there is a sudden explosion of flesh and blood as the strain of the pear segments exceeds the eleastic limit of his skin.{x
	mpecho {wSogesk gurgles and sobs from his destroyed mouth as the chunky, ruined ribbons of his face float about him and clouds of dark blood fill the water about his head.{x
	emote shivers with pleasure and stops turning the key.
	say Nothing old Nirlisk can't take care of, Sogesk!  Cheer up!
	mpecho Sogesk hangs his head, choking and sobbing miserably.
	mpecho {wTestris takes a few deep breaths to regain his composure.{x
	say Always happy to demonstrate our products for such a discerning customer, $n.
	mpecho {wTestris guides you back down to the shop's lower level.{x
	mptransfer $n 27346
	mpgoto 27346
	mpforce $n look
	mpoload 27359 30
	say Use it always in good health.
	give anguish $n
	say A pleasure to serve, $n.
~
>bribe_prog 650~
	wake
	mpjunk coins
	say Ah, a classic.  This way!
	mpecho {wYou follow Testris up to the second level of the building.{x
	mptransfer $n 27646
	mpgoto 27646
	mpforce $n look
	mpecho {wTestris snaps his thumb and forefinger and his assistant Sogesk slopes dejectedly into the room.{x
	say Today, Sogesk, we will demonstrate the correct way to utilise an instep borer.
	mpecho Sogesk whimpers.
	mpecho {wTestris directs Sogesk to the large wooden chair, uses the restraints on it to immobilise him, and retrieves what looks like a large iron boot from a nearby cupboard.{x
	mpecho Sogesk hangs his head, resigned to what is to come.
	say The instep borer externally resembles an iron boot.  It is hinged to permit the free insertion and removal of the bare foot.
	say Foot please, Sogesk.
	leer
	mpecho Sogesk meekly offers his right foot.
	say Wonderful.  Many thanks.
	mpecho {wTestris unhinges the device and clamps it over Sogesk's trembling foot.{x
	say You can see here that there is a crank projecting from a housing over the instep. It conceals a long, thick, serrated iron blade, which has been grooved so as to inflict maximum damage to the victim, and to promote a very liberal flow of blood.
	mpecho {wYou see Sogesk physically brace himself.{x
	lick
	say Showtime, Sogesk!
	mpecho {wTestris starts to turn the crank.  Nothing seems to happen for a few seconds, then suddenly -{x
	mpecho {wSogesk wails, and you hear a wet CRUNCH as his instep is punctured by the blade.{x
	mpecho {wTestris cackles gleefully and continues to turn the crank.{x
	mpecho {wSogesk screams at the top of his lungs as thick clouds of blood float up from the top of the boot.{x
	mpecho {wAfter several minutes of this Sogesk passes out.{x
	emote beams delightedly
	say The resulting wound produced by the blade is so large that it is quite common for its victims to die of toxaemia shortly after!
	mpecho Sogesk's body twitches.
	mpecho {wTestris takes his hand off the crank and takes a few moments to catch his breath.{x
	say Always a pleasure to demonstrate our products for you $n, I hope you enjoyed the show.  After you.
	mpecho {wTestris escorts you back down to the shop.{x
	mptransfer $n 27346
	mpgoto 27346
	mpforce $n look
	mpoload 27358 30
	say With our warmest regards.
	give borer $n
	say Thank you for your custom!
~
>bribe_prog 625~
	wake
	mpjunk coins
	say With pleasure! Follow me.
	mpecho {wTestris escorts you to the second level of Torturers' Delight.{x
	mptransfer $n 27646
	mpgoto 27646
	mpforce $n look
	mpecho {wTestris coughs and Sogesk, his hapless assistant, swims into the room.{x
	say Sogesk will help me to demonstrate the proper use and application of a tongue tearer.
	mpecho {wTestris orders Sogesk to sit in a large wooden chair covered in straps, fastens the restraints, and retrieves what looks like a pair of sharpened tongs from a mount on the wall.{x
	mpecho Sogesk gulps.
	say In appearance, a tongue tearer resembles a large pair of scissors crossed with tongs or pliers.  They may be used with or without the prior application of heat.
	say Shall we heat it up today, Sogesk?
	grin
	mpecho Sogesk shudders; his eyes twitch involuntarily.
	say Splendid, then!
	mpecho {wTestris places the tearer in a small (presumably) magical brazier next to the rack.{x
	say There are a number of variations on the basic design.  Some function more like scissors, slicing the victim's tongue up into ribbons, while others more like pliers, being designed to grasp the tongue and rip it out whole.
	mpecho {wSogesk shakes, his twitching eyes fixed on the white-hot tongue tearer resting in the brazier.{x
	lick
	say This one's a ripper.  Open wide, Sogesk!
	mpecho {wTestris retrieves the tearer from the brazier and moves towards the immobilised Sogesk.{x
	mpecho {wSogesk screams, vainly trying to wriggle away from the glowing tongs.{x
	mpecho {wTestris gleefully forces the tearer into Sogesk's mouth.{x
	mpecho {wSogesk makes an awful gurgling, sizzling sound and the room quickly fills with the fishy stench of burning sahuagin.{x
	mpecho {wAfter a few seconds Testric pulls the tearer out of Sogesk's mouth.  A still-wriggling, severed blue tongue is in its grippers.{x
	emote smiles happily
	mpecho Sogesk passes out and slumps back into the chair, blood gushing from his ruined maw.
	mpecho {wTestris coughs, struggling to regain his composure and assume a professional demeanour.{x
	say As always, $n, I hope you enjoyed this brief demonstration of one of our products.  Please follow me.
	mpecho {wTestris escorts you back down to the bottom level of Torturers' Delight.{x
	mptransfer $n 27346
	mpgoto 27346
	mpforce $n look
	mpoload 27357 30
	say For you.
	give tearer $n
	say Please come again!
~
>bribe_prog 600~
	wake
	mpjunk coins
	say Certainly! Follow me.
	mpecho {wTestris escorts you to the second level of Torturers' Delight.{x
	mptransfer $n 27646
	mpgoto 27646
	mpforce $n look
	mpecho {wTestris whistles and a small, forlorn-looking sahuagin male swims into the room.{x
	say Sogesk here is my assistant, and will help me to demonstrate the use of the thumbscrew.
	mpecho {wTestris places Sogesk's right thumb into the thumbscrew.{x
	say The thumbscrew here is a simple vice, with the addition of sharp metal points -
	mpecho {w... Testris shows you the undersides of the bars holding Sogesk's thumb...{x
	say - under the crushing bars that puncture your victim's nail and really 'bring the pain,' as it were, by working into the flesh of the nail bed once pressure from the vice is applied.
	mpecho {wSogesk whimpers.{x
	grin
	say You can imagine the sheer continuum of suffering that can be produced by judicious use of this simple device.  From considerable discomfort -
	mpecho {wTestris turns the wingnut on the thumbscrew a few times...{x
	mpecho {wSogesk yelps.{x
	say - to a level of pain generally considered unbearable, and likely to produce enthusiastic confessions from victims and/or inspire great delight in the torturer, as might be appropriate to the purpose or purposes of the session.
	mpecho {wTestris turns the wingnut on the thumbscrew several dozen times...{x
	mpecho {wA number of bood-curdling screams issue from Sogesk, and blood starts to gush from his thumb.{x
	emote chuckles as Sogesk's screams reach a bone-chilling crescendo before unwinding the wingnut.
	mpecho {wSogesk extracts his mangled thumb from the thumbscrew and cradles it in his other hand, moaning piteously.{x
	say I hope you enjoyed this brief demonstration.  Please follow me back to the shop floor.
	mpecho {wTestris escorts you back to Torturers' Delight's bottom level.{x
	mptransfer $n 27346
	mpgoto 27346
	mpforce $n look
	mpoload 27356 30
	say A token.
	give thumbscrew $n
	say Enjoy.
~
>bribe_prog 1~
wake
emote looks at the miserable quantity of coin you gave him
say Please.
say Read the SIGN and try again.
~
|
