CC      = gcc
PROF    =
DEBUG   = -g
NOCRYPT =
LINK    =
O_FLAGS = -O2
C_FLAGS = $(O_FLAGS) -pedantic-errors -Wall \
          -Wno-trigraphs -Wno-unused-result -Wno-overlength-strings -Wno-overflow \
          $(DEBUG) $(PROF) $(NOCRYPT) -Dlinux -Dunix
L_FLAGS = $(LINK) $(DEBUG) $(PROF) -lcrypt -lm

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o board.o comm.o const.o \
          create.o healer.o hunt.o db.o fight.o handler.o interp.o magic.o mob_commands.o \
          mob_prog.o save.o special.o update.o quest.o trap.o skill.o sft.o table.o \
          deity.o tournament.o bot.o

dd4: $(O_FILES)
	-rm -f dd4
	$(CC) -o dd4 $(O_FILES) $(L_FLAGS)

new: $(O_FILES)
	-rm -f dd4.new
	$(CC) -o dd4.new $(O_FILES) $(L_FLAGS)

const.o: const.c pre_reqs/*.c merc.h

%.o: %.c merc.h
	$(CC) -c $(C_FLAGS) $<

clean:
	-rm *.o dd4
