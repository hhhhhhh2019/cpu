#include "cc.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


char* input_filename = NULL;
char* output_filename = "a.S";


int main(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			print_help();
			return 1;
		}

		else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
			output_filename = argv[++i];
		}

		else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
			input_filename = argv[++i];
		}

		else {
			input_filename = argv[i];
		}
	}


	if (input_filename == NULL) {
		printf("Expected input filename\n");
		print_help();
		return 1;
	}


	Compiler_state state = {
		.output_filename = output_filename,
		// .map_filename    = map_output_filename,
		// .macros_count  = 0,
		// .macros        = malloc(0),
		// .defines_count = 0,
		// .defines       = malloc(0),
	};


	FILE* f = fopen(input_filename, "r");

	fseek(f, 0, SEEK_END);
	unsigned int file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* file_data = malloc(file_size + 1);
	fread(file_data, file_size, 1, f);
	file_data[file_size] = 0;

	fclose(f);

	state.lexer_result = lexer(file_data);

	LOG("\n");
	LOG("lexer result:\n");

	for (int i = 0; i < state.lexer_result.tokens_count; i++) {
		if (state.lexer_result.tokens[i].type != NEWLINE) {
			LOG("%s %s\n",
			    token_type_names[state.lexer_result.tokens[i].type],
			    state.lexer_result.tokens[i].value);
			}
		else
			LOG("NEWLINE\n");
	}
}


void print_help() {
}
