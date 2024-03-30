#include <as.h>
#include <bin.h>
#include <map.h>
#include <error.h>
#include <utils.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


char* input_filename = "../bios.S";
char* output_filename = "a.out";
char* map_output_filename = NULL;


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

		else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--map") == 0) {
			map_output_filename = argv[++i];
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
		.map_filename    = map_output_filename,
		.macros_count  = 0,
		.macros        = malloc(0),
		.defines_count = 0,
		.defines       = malloc(0),
	};

	state.lexer_result = preprocess(&state, input_filename);

	print_errors();

	LOG("\n");
	LOG("preprocess result:\n");

	for (int i = 0; i < state.lexer_result.tokens_count; i++) {
		if (state.lexer_result.tokens[i].type != NEWLINE)
			LOG("%s %s\n",
			    token_type_names[state.lexer_result.tokens[i].type],
			    state.lexer_result.tokens[i].value);
		else
			LOG("NEWLINE\n");
	}

	synt(&state);

	print_errors();
	
	// for (int i = 0; i < state.synt_result.nodes_count; i++)
	// 	create_dot_from_node(state.synt_result.nodes[i]);
	
	semant(&state);

	print_errors();

	if (map_output_filename != NULL)
		map(&state);

	// free_all_nodes();
	// free_all_alloced_names();
	
	bin(&state);

	print_errors();
}



void print_help() {
}
