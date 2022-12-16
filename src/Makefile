IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lm -lrt -lasound -ljack -lpthread -lportaudio

_DEPS = sound-linux.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = sound-linux.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

example: $(OBJ)
	$(CC) avr-sound-example.c -o $@ $^ $(CFLAGS) $(LIBS) && ./example

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~