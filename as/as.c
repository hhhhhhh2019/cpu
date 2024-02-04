#include <as.h>
#include <error.h>
#include <utils.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


char* input_filename = "../bios.S";
char* output_filename = "a.out";


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
		printf("Expected input filename");
		print_help();
		return 1;
	}


	Compiler_state state = {
		.macros_count = 0,
		.macros = malloc(0),
		.defines_count = 0,
		.defines = malloc(0),
	};

	state.lexer_result = preprocess(&state, input_filename);

	print_errors();

	LOG("\n");
	LOG("preprocess result:\n");

	for (int i = 0; i < state.lexer_result.tokens_count; i++) {
		if (state.lexer_result.tokens[i].type != NEWLINE)
			LOG("%s\n", state.lexer_result.tokens[i].value);
	}
}



void print_help() {
}
