LIBS = 

CC_FLAGS=-c -I ./include -DDEBUG -g #`pkg-config --cflags $(LIBS)`
LD_FLAGS= #`pkg-config --libs $(LIBS)`

#  -fsanitize=undefined -fno-sanitize=alignment -fsanitize=address
#  -lasan -lubsan

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
