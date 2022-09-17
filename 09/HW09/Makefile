#CFLAGS+= -Wall -Werror -std=c99 -g
CFLAGS+= -Wall -Werror -g

LDFLAGS+=-lpthread

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

PRG_SERIAL=prg_serial.o

BINS=lab09-main
bin: ${BINS}

lab09-main: hw09-main.o prg_serial.o
	${CC} $< ${PRG_SERIAL} ${LDFLAGS} -o $@

${OBJS}: %.o: %.c
	${CC} ${CFLAGS} ${CPPFLAGS} -c $< -o $@

clean:
	rm -f *.o $(OBJS) ${BINS} 
