#ifndef UTILS_H
#define UTILS_H


#define Stack(T) typedef struct { \
	T* values; \
	unsigned long values_count; \
} Stack_##T;


#define stack_push(stack, val) \
	do { \
		stack.values = realloc(stack.values, sizeof(*stack.values) * (++stack.values_count)); \
	stack.values[stack.values_count-1] = val; \
	} while (0)

#define stack_pop(stack) \
	stack.values[stack.values_count-- - 1]


#endif
