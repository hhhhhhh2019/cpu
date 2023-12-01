#ifndef BIN_H
#define BIN_H


#include <as.h>


typedef struct Bin_result {
	char* data;
	unsigned int data_size;
} Bin_result;


Bin_result bin_parse(Parser_state*);


#endif
