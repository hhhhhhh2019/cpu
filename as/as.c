#include <as.h>
#include <utils.h>
#include <bin.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>


char* input_filename = NULL;
char* output_filename = "a.out";


void print_help();


int main(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			print_help();
			return 0;
		}

		else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) {
			output_filename = argv[++i];
		}

		else {
			input_filename = argv[i];
		}
	}


	if (input_filename == NULL) {
		printf("No input filename! --help to show help message\n");
		return 1;
	}


	lexer_init();
	prepeare_todo();
	prepeare_names();


	Parser_state state = parse_file_stage_1(input_filename);


	synt_parse(&state);

	if (state.ok == 0)
		return 1;

	collapse_node(state.sresult.root);


	semant_parse(&state);

	if (state.ok == 0)
		return 1;


	//create_dot_from_node(state.sresult.root);


	Bin_result bin = bin_parse(&state);


	if (state.ok == 0)
		return 1;


	FILE* f = fopen(output_filename, "wb");

	if (f == NULL) {
		perror("fopen");
		return 1;
	}

	fwrite(bin.data, bin.data_size, 1, f);

	fclose(f);


	free(bin.data);


	free_state(state);


	free_names();
	free_todo();
}


void print_help() {
	
}


void free_state(Parser_state state) {
	free(state.dirname);
	free(state.filepath);
	free(state.filedata);


	for (int i = 0; i < state.lresult.real_tokens_count; i++)
		free(state.lresult.real_tokens[i].value);

	free(state.lresult.real_tokens);
	free(state.lresult.tokens);

	free_node(state.sresult.root);

	free(state.seresult.labels);
}



Parser_state parse_file_stage_1(char* filepath) {
	Parser_state state = {.ok=1};


	FILE* f = fopen(filepath, "r");
	if (f == NULL) {
		printf("Error: can't open file \"%s\", errno: %d\n", state.filepath, errno);
		state.ok = 0;
		return state;
	}

	fseek(f, 0, SEEK_END);
	state.filedata_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	state.filedata = calloc(state.filedata_size + 1, 1);
	fread(state.filedata, state.filedata_size, 1, f);

	fclose(f);

	state.filepath = realpath(filepath, NULL);
	state.filename = basename(state.filepath);
	state.dirname  = malloc(strlen(state.filepath)+1);
	strcpy(state.dirname, state.filepath);
	dirname(state.dirname);


	// LOG("==== start: %s ===============================\n", state.filepath);


	lexer_parse(&state);

	/* LOG("  --- %s lexer ----------------------------------------\n", state.filename);
	for (int i = 0; i < state.lresult.tokens_count; i++)
		LOG("   %2lu:%2lu %2d %s\n",
		   state.lresult.tokens[i].line,
		   state.lresult.tokens[i].col,
		   state.lresult.tokens[i].type,
		   state.lresult.tokens[i].value); */

	return state;
}
