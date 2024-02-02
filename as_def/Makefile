LIBS = 

CC_FLAGS=-c -I ./include -DDEBUG  -g #`pkg-config --cflags $(LIBS)`
LD_FLAGS=-lm#`pkg-config --libs $(LIBS)`

# CC_FLAGS+=-fsanitize=address #-fsanitize=undefined -fno-sanitize=alignment
# LD_FLAGS+=-lasan -lubsan -lm

CC ?= gcc
LD = $(CC)

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)


%.o: %.c
	$(CC) $(CC_FLAGS) $< -o $@

all: $(OBJECTS)
	$(LD) $^ $(LD_FLAGS) -o as


clean:
	rm *.o as -f
