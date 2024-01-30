#include <cc.h>
#include <lexer.h>
#include <synt.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>


char* input_filename = "../code.c";//NULL;
char* output_filename = "a.out";


char* error_names_ru[] = {
	[CANT_OPEN_FILE] = "Ошибка",
	[SYNTAX_ERROR] = "Синтаксическая ошибка"
};

char** error_names;


int main(int argc, char** argv) {
	error_names = error_names_ru;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			print_help();
			return 0;
		}

		else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
			output_filename = argv[++i];
		}

		else {
			input_filename = argv[i];
		}
	}

	if (input_filename == NULL) {
		printf("No input filename\n");
		return 1;
	}

	lexer_init();
	// prepeare_todo();
	prepeare_names();

	Compiler_state state = parse_file_prepoc(input_filename);

	if (print_errors(&state))
		free_and_exit(state, 1);


	lexer(&state);


	/* for (int i = 0; i < state.tokens_count; i++) {
		printf("   %2lu:%2lu %3d %s\n",
		   state.tokens[i].line,
		   state.tokens[i].col,
		   state.tokens[i].type,
		   state.tokens[i].value);
	} */


	synt(&state);

	if (print_errors(&state))
		free_and_exit(state, 1);

	create_dot_from_node(state.AST);


	semant(&state);


	free_and_exit(state, 0);
}


Compiler_state parse_file_prepoc(char* filepath) {
	Compiler_state state = {
		.errors_count = 0,
		.errors = malloc(0),

		.code_size = 0,
		.code = NULL,

		.tokens_count = 0,
		.tokens = malloc(0),

		.real_tokens_count = 0,
		.real_tokens = malloc(0),

		.filepath = NULL,
		.filename = NULL,
		.dirname = NULL
	};

	FILE* f = fopen(filepath, "r");
	if (f == NULL) {
		Error error = {
			.type = CANT_OPEN_FILE,
			.text = malloc(1000),
		};
		sprintf(error.text, "Can't open file \"%s\": %s", filepath, strerror(errno));
		add_error(&state, error);
		return state;
	}

	fseek(f, 0, SEEK_END);
	state.code_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	state.code = calloc(state.code_size + 1, 1);
	fread(state.code, state.code_size, 1, f);

	fclose(f);

	state.filepath = realpath(filepath, NULL);
	state.filename = basename(state.filepath);
	state.dirname  = malloc(strlen(state.filepath)+1);
	strcpy(state.dirname, state.filepath);
	dirname(state.dirname);

	return state;
}


void print_help() {
	printf("help message\n");
}


void add_error(Compiler_state* state, Error error) {
	state->errors = realloc(state->errors, sizeof(Error) * (++state->errors_count));
	state->errors[state->errors_count-1] = error;
}


char print_errors(Compiler_state* state) {
	if (state->errors_count == 0)
		return 0;

	for (int i = 0; i < state->errors_count; i++)
		printf("\e[1;31m%s:\e[m %s\n", error_names[state->errors[i].type], state->errors[i].text);

	return 1;
}


void free_and_exit(Compiler_state state, int code) {
	for (int i = 0; i < state.errors_count; i++)
		free(state.errors[i].text);

	free(state.code);

	free(state.filepath);
	free(state.dirname);

	exit(code);
}
