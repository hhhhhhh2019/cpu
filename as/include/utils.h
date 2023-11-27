#ifndef UTILS_H
#define UTILS_H


#include <stdlib.h>


#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif


typedef struct Stack_long {
	long* values;
	unsigned int size;
} Stack_long;

static inline void stack_long_push(Stack_long* stack, long value) {
	stack->values = realloc(stack->values, sizeof(long) * (++stack->size));
	stack->values[stack->size-1] = value;
}

static inline long stack_long_pop(Stack_long* stack) {
	return stack->values[--stack->size];
}



long dec2long(char*);
long hex2long(char*);
long bin2long(char*);

long get_long_size(long);


#endif
