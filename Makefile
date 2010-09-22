CFLAGS = -Wall
RM = rm -f

# Note: to build for Windows, uncomment the following line
# LDFLAGS = -lwinmm

OBJS =	\
	pong.o \
	timer.o \
	command_options.o \
	titlescreen.o \
	keyboard.o
# end of OBJS

pong.exe: $(OBJS)
	gcc $(OBJS) $(LDFLAGS) -o pong.exe

pong.o: pong.c timer.h command_options.h titlescreen.h
	gcc $(CFLAGS) -c pong.c

timer.o: timer.c timer.h
	gcc $(CFLAGS) -c timer.c

command_options.o: command_options.c command_options.h
	gcc $(CFLAGS) -c command_options.c

titlescreen.o: titlescreen.c titlescreen.h
	gcc $(CFLAGS) -c titlescreen.c

keyboard.o: keyboard.c keyboard.h
	gcc $(CFLAGS) -c keyboard.c

clean_obj:
	${RM} *.o

clean: clean_obj
	${RM} pong.exe
