#include "lexer.h"
#include "preproc.h"
#include <as.h>
#include <utils.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>



static char* find_include_file(Compiler_state* state, char* file, char* dirname) {
	// check in current dir
	
	char* path = malloc(strlen(file) + strlen(dirname) + 1);

	strcpy(path, dirname);
	strcat(path, file);

	if (access(path, F_OK) == 0) {
    return path;
  }

	// check in -I paths
	

	return NULL;
}


static unsigned long parse_macro(
    Compiler_state* state, Token* tokens, unsigned long tokens_count) {
	Macro macro = {
		.name = tokens[1].value,
		.args_count = 0,
		.args = malloc(0),
		.tokens_count = 0,
		.tokens = malloc(0)
	};

	unsigned long offset = 2;

	for (; offset < tokens_count; offset++) {
		if (tokens[offset].type == RIGHT_PAREN)
			break;

		if (tokens[offset].type != UNDEFINED) {
			// error
			break;
		}

		macro.args = realloc(macro.args, sizeof(Token) * (++macro.args_count));
		macro.args[macro.args_count - 1] = tokens[offset];

		if (tokens[offset + 1].type == COMMA)
			offset += 2;
	}

	for (; offset < tokens_count; offset++) {
		Token t = tokens[offset];

		if (t.type == NEWLINE)
			break;

		if (t.type == BACK_SLASH) {
			t = tokens[++offset];

			if (t.type == NEWLINE)
				continue;

			// error
		}

		macro.tokens = realloc(macro.tokens, sizeof(Token) * (++macro.tokens_count));
		macro.tokens[macro.tokens_count - 1] = t;
	}

	state->macros = realloc(state->macros, sizeof(Macro) * (++state->macros_count));
  state->macros[state->macros_count - 1] = macro;

	return offset;
}


static unsigned long parse_define(
    Compiler_state* state, Token* tokens, unsigned long tokens_count) {
	if (tokens[1].type == LEFT_PAREN)
		return parse_macro(state, tokens, tokens_count);

	Define define = {
		.name = tokens[1].value,
		.tokens_count = 0,
		.tokens = malloc(0)
	};

	unsigned long offset = 2;

	for (; offset < tokens_count; offset++) {
		Token t = tokens[offset];

		if (t.type == NEWLINE)
			break;

		if (t.type == BACK_SLASH) {
			t = tokens[++offset];

			if (t.type == NEWLINE)
				continue;

			// error
		}

		define.tokens = realloc(define.tokens, sizeof(Token) * (++define.tokens_count));
		define.tokens[define.tokens_count - 1] = t;
	}

	state->defines = realloc(state->defines, sizeof(Macro) * (++state->defines_count));
  state->defines[state->defines_count - 1] = define;

	return offset;
}


static void expand_define(
    Compiler_state* state, Define define, Token* tokens, unsigned long tokens_count,
    Token** result_tokens, unsigned long* result_tokens_count) {
	*result_tokens_count = define.tokens_count;
	*result_tokens = malloc(sizeof(Token) * *result_tokens_count);

	for (int i = 0; i < define.tokens_count; i++)
		(*result_tokens)[i] = define.tokens[i];
}


static void expand_macro(
    Compiler_state* state, Macro macro, Token* tokens, unsigned long tokens_count,
    Token** result_tokens, unsigned long* result_tokens_count) {

}


static char check_macros(
    Compiler_state* state, Token* tokens, unsigned long tokens_count,
    Token** result_tokens, unsigned long* result_tokens_count) {
	for (int i = 0; i < state->defines_count; i++) {
		if (strcmp(tokens[0].value, state->defines[i].name) != 0)
			continue;

		expand_define(state, state->defines[i], tokens, tokens_count,
		              result_tokens, result_tokens_count);

		return 1;
	}

	for (int i = 0; i < state->macros_count; i++) {
		if (strcmp(tokens[0].value, state->macros[i].name) != 0)
			continue;

		expand_macro(state, state->macros[i], tokens, tokens_count,
		             result_tokens, result_tokens_count);

		return 1;
	}

	return 0;
}


static char preproc(Compiler_state* state, Lexer_result* lexer_result, char* dirname) {
	unsigned long tokens_count = 0;
	Token* tokens = malloc(0);

	char result = 0;


	for (int i = 0; i < lexer_result->tokens_count; i++) {
		Token t = lexer_result->tokens[i];

		if (t.type == INCLUDE) {
			result = 1;

			Token path_token = lexer_result->tokens[++i];
			char* path = find_include_file(state, path_token.value, dirname);

			if (path == NULL) {
				ERROR("File \"%s\" not found\n", path_token.value);
				continue;
			}

			Lexer_result new_file = preprocess(state, path);
			i++;

			for (int j = 0; j < new_file.tokens_count; j++) {
				tokens = realloc(tokens, sizeof(Token) * (++tokens_count));
				tokens[tokens_count - 1] = new_file.tokens[j];
			}

			free(new_file.tokens);

			free(t.value);
			free(path_token.value);

			continue;
		}

		if (t.type == DEFINE) {
			result = 1;

			i += parse_define(state, &lexer_result->tokens[i], lexer_result->tokens_count - i);

			continue;
		}

		Token* new_tokens;
		unsigned long new_tokens_count;

		if (check_macros(state,
		        &lexer_result->tokens[i], lexer_result->tokens_count - i,
		        &new_tokens, &new_tokens_count)) {
			result = 1;

			for (int j = 0; j < new_tokens_count; j++) {
				tokens = realloc(tokens, sizeof(Token) * (++tokens_count));
				tokens[tokens_count - 1] = new_tokens[j];
			}

			free(new_tokens);

			continue;
		}

		tokens = realloc(tokens, sizeof(Token) * (++tokens_count));
		tokens[tokens_count - 1] = t;
	}


	lexer_result->tokens = tokens;
	lexer_result->tokens_count = tokens_count;

	return result;
}


Lexer_result preprocess(void* vstate, char* filename) {
	LOG("preprocess: %s\n", filename);

	char* rp = realpath(filename, NULL);
	char* dn = malloc(strlen(rp)+1);
	strcpy(dn, rp);
	dirname(dn);
	unsigned long dn_len = strlen(dn);
	dn[dn_len+1] = 0;
	dn[dn_len] = '/';


	FILE* file = fopen(filename, "r");

  if (file == NULL) {
		ERROR("Cannot open file \"%s\": %s\n", filename, strerror(errno));
#ifdef DEBUG
		perror("fopen " STR(__FILE__) " " STR(__LINE__));
#endif
		exit(errno);
  }

	fseek(file, 0, SEEK_END);
	unsigned long size = ftell(file);
	fseek(file, 0, SEEK_SET);
 
	char* data = malloc(size+1);
	data[size] = 0;

	fread(data, size, 1, file);

	LOG("lexer start: %s\n", filename);
	Lexer_result lexer_result = lexer(data);
	LOG("lexer end: %s\n", filename);

	while (preproc(vstate, &lexer_result, dn)) {}

	return lexer_result;
}
