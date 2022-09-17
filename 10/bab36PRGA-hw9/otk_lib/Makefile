CFLAGS+=-I/usr/local/include

LIB=libotk.a

all:  lib

lib: $(LIB)

$(LIB): otk_lib.o gadget_lib.o
	$(AR) rcs $(LIB) $^

otk_lib.o:  otk_lib.c otk_lib.h
	$(CC) -c -O $(CFLAGS) $< -o $@

gadget_lib.o:  gadget_lib.c gadget_lib.h
	$(CC) -c -O $< $(CFLAGS) -o $@

clean:
	rm -f otk_lib.o gadget_lib.o
