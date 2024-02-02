#include <as.h>
#include <preproc.h>

#include <stdio.h>
#include <string.h>


char* input_filename = NULL;
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


	Compiler_state state;


	state.lexer_result = preprocess(input_filename);
}



void print_help() {
}
