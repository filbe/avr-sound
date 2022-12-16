
LIBS=-lm -lrt -lasound -ljack -lpthread -lportaudio
CC=gcc
SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib

SOURCES = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
LIB_SOURCES = $(wildcard $(LIB_DIR)/*.c)

OBJECTS = $(SOURCES:.c=.o) $(LIB_SOURCES:.c=.o)

EXECUTABLE = example

CFLAGS = -I$(INCLUDE_DIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)