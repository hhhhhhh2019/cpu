LIBS = 

CC_FLAGS=-c -I ./include -DDEBUG  -g #`pkg-config --cflags $(LIBS)`


# CC_FLAGS+=-fsanitize=address
# LD_FLAGS+=-lasan

# CC_FLAGS+=-fsanitize=undefined -fno-sanitize=alignment
# LD_FLAGS+=-lubsan -lm

# LD_FLAGS=`pkg-config --libs $(LIBS)`


CC ?= gcc
LD = $(CC)

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)


%.o: %.c
	$(CC) $(CC_FLAGS) $< -o $@

all: $(OBJECTS)
	$(LD) $^ $(LD_FLAGS) -o cc


clean:
	rm *.o cc -f
