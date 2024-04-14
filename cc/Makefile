LIBS =
DEBUG = 1
SANITIZER = 0

OUTPUT = cc

CC_FLAGS = -c -I ./include
LD_FLAGS = -lm


ifneq ($(LIBS),)
	CC_FLAGS += `pkg-config --cflags $(LIBS)`
	LD_FLAGS += `pkg-config --libs $(LIBS)`
endif


ifeq ($(DEBUG),1)
	CC_FLAGS += -g -DDEBUG
endif


ifeq ($(SANITIZER),1)
	CC_FLAGS += -fsanitize=address
	LD_FLAGS += -l asan
endif


SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)


%.o: %.c
	$(CC) $(CC_FLAGS) $< -o $@


all: $(OBJECTS)
	$(CC) $^ $(LD_FLAGS) -o $(OUTPUT)


clean:
	rm *.o $(OUTPUT) -f
