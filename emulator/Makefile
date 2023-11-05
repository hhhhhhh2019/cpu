LIBS = 

CC_FLAGS=-c -I ./include -DDEBUG -fsanitize=undefined -fno-sanitize=alignment -fsanitize=address #`pkg-config --cflags $(LIBS)`
LD_FLAGS=-lasan -lubsan #`pkg-config --libs $(LIBS)`

CC ?= gcc
LD = $(CC)

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)


%.o: %.c
	$(CC) $(CC_FLAGS) $< -o $@

all: $(OBJECTS)
	$(LD) $^ $(LD_FLAGS) -o emulator


clean:
	rm *.o emulator -f
