#ifndef UTILS_H
#define UTILS_H


#include <as.h>
#include <synt.h>

#include <stdio.h>


#define STR_HELP(x) #x
#define STR(x) STR_HELP(x)


#ifdef DEBUG
#define LOG(...) do { \
	printf("\033[38;5;4mInfo:\033[0m "); printf(__VA_ARGS__); \
} while(0)
#else
#define LOG(...)
#endif

#define ERROR(...) do { \
	printf("\033[38;5;1mError:\033[0m "); printf(__VA_ARGS__); \
} while(0)


long solve_expression(Compiler_state*, char allow_labels, char*, Node*, unsigned long id);

char get_reg_id(char*);


#endif
