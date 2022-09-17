#CFLAGS+= -Wall -Werror -std=gnu99 -g
LDFLAGS=-pthread

BIN=prga-hw9

CPPFLAGS+=-Iotk_lib -I/usr/local/include
CFLAGS+=-O2

#1st local otk_lib
LDFLAGS+=-Lotk_lib -lotk

LDFLAGS+=-L/usr/local/lib 
LDFLAGS+=-lX11 -lGL -lGLU -lXmu -lm

all: bin

bin: $(BIN)

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

$(BIN): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJS): %.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(OBJS)
