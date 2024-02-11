#include <as.h>
#include <error.h>
#include <utils.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>


Stack(char);


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

	unsigned long offset = 3;

	for (; offset < tokens_count;) {
		if (tokens[offset].type == RIGHT_PAREN)
			break;

		if (tokens[offset].type != UNDEFINED) {
			// error
			break;
		}

		macro.args = realloc(macro.args, sizeof(Token) * (++macro.args_count));
		macro.args[macro.args_count - 1] = tokens[offset++];

		if (tokens[offset].type == COMMA)
			offset++;
	}
	

	if (tokens[offset].type != RIGHT_PAREN) {
		add_error((Error){
				.type = EXPECT_TOKEN,
				.token = tokens[offset],
		    .excepted_token = RIGHT_PAREN
		});
		return 0;
	}

	offset++;


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


	char redefine = 0;


	for (int i = 0; i < state->defines_count; i++) {
		if (strcmp(macro.name, state->defines[i].name) == 0)
			redefine = 1;
	}

	for (int i = 0; i < state->macros_count; i++) {
		if (strcmp(macro.name, state->macros[i].name) != 0)
			redefine = 1;
	}


	if (redefine) {
		add_error((Error){
			.type = PREPROCESS_REDEFINITION,
			.token = tokens[1]
		});
	} else {
		state->macros = realloc(state->macros, sizeof(Macro) * (++state->macros_count));
		state->macros[state->macros_count - 1] = macro;
	}

	return offset;
}


static unsigned long parse_define(
    Compiler_state* state, Token* tokens, unsigned long tokens_count) {
	if (tokens[2].type == LEFT_PAREN)
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


	char redefine = 0;


	for (int i = 0; i < state->defines_count; i++) {
		if (strcmp(define.name, state->defines[i].name) == 0)
			redefine = 1;
	}

	for (int i = 0; i < state->macros_count; i++) {
		if (strcmp(define.name, state->macros[i].name) != 0)
			redefine = 1;
	}


	if (redefine) {
		add_error((Error){
			.type = PREPROCESS_REDEFINITION,
			.token = tokens[1]
		});
	} else {
		state->defines = realloc(state->defines, sizeof(Macro) * (++state->defines_count));
		state->defines[state->defines_count - 1] = define;
	}

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


static void expand_macros(
    Compiler_state* state, Macro macro, Token* tokens, unsigned long tokens_count,
    Token** result_tokens, unsigned long* result_tokens_count, unsigned long* noffset) {
	if (tokens[1].type != LEFT_PAREN) {
		add_error((Error){
		    .type = EXPECT_TOKEN,
		    .token = tokens[1],
		    .excepted_token = LEFT_PAREN
		});
		return;
	}

	*result_tokens_count = 0;
	*result_tokens = malloc(0);

	unsigned long offset = 2;

	unsigned long args_count = macro.args_count;
	Token* args = malloc(sizeof(Token) * args_count);

	for (int i = 0; i < macro.args_count; i++) {
		if (tokens[offset].type == RIGHT_PAREN) {
			add_error((Error){
					.type = UNEXPECTED_TOKEN,
					.token = tokens[2 + i],
			});
			return;
		}

		args[i] = tokens[offset++];

		if (tokens[offset].type == COMMA)
			offset++;
	}

	if (tokens[offset].type != RIGHT_PAREN) {
		add_error((Error){
				.type = EXPECT_TOKEN,
				.token = tokens[offset],
		    .excepted_token = RIGHT_PAREN
		});
		return;
	}


	for (int i = 0; i < macro.tokens_count; i++) {
		Token t = macro.tokens[i];

		for (int j = 0; j < macro.args_count; j++) {
			if (strcmp(macro.tokens[i].value, macro.args[j].value) != 0)
				continue;

			t = args[j];
			break;
		}

		*result_tokens = realloc(*result_tokens, sizeof(Token) * (++(*result_tokens_count)));
		(*result_tokens)[(*result_tokens_count) - 1] = t;

		offset++;
	}


	*noffset = macro.tokens_count;
}


static char check_macros(
    Compiler_state* state, Token* tokens, unsigned long tokens_count,
    Token** result_tokens, unsigned long* result_tokens_count, unsigned long* offset) {
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

		expand_macros(state, state->macros[i], tokens, tokens_count,
		              result_tokens, result_tokens_count, offset);

		return 1;
	}

	return 0;
}


static char preproc(Compiler_state* state, Lexer_result* lexer_result, char* dirname) {
	unsigned long tokens_count = 0;
	Token* tokens = malloc(0);

	char result = 0;


	Stack_char if_stack = {
	    .values = malloc(0),
	    .values_count = 0
	};


	for (int i = 0; i < lexer_result->tokens_count; i++) {
		Token t = lexer_result->tokens[i];

		if (t.type == ENDIF) {
			stack_pop(if_stack);
			continue;
		}

		if (t.type == ELSE) {
			stack_push(if_stack, 1 - stack_pop(if_stack));
			continue;
		}


		if (if_stack.values_count != 0) {
			if (if_stack.values[if_stack.values_count-1] == 0)
				continue;
		}


		if (t.type == INCLUDE) {
			result = 1;

			if (i == lexer_result->tokens_count - 1 ||
			    lexer_result->tokens[i+1].type != STRING) {
        add_error((Error){
            .type = EXPECT_TOKEN,
				    .token = lexer_result->tokens[i],
				    .excepted_token = STRING
        });
        continue;
      }

			Token path_token = lexer_result->tokens[++i];
			char* path = find_include_file(state, path_token.value, dirname);

			if (path == NULL) {
				add_error((Error){
				    .type = FILE_NOT_FOUND,
				    .token = path_token
				});
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

		if (t.type == IF) {
			result = 1;
			continue;
		}

		if (t.type == IFDEF) {
			result = 1;

			Token t2 = lexer_result->tokens[++i];

			char find = 0;

			for (int i = 0; i < state->defines_count; i++) {
				if (strcmp(t2.value, state->defines[i].name) != 0)
					continue;

				find = 1;
			}

			for (int i = 0; i < state->macros_count; i++) {
				if (strcmp(t2.value, state->macros[i].name) != 0)
					continue;

				find = 1;
			}

			stack_push(if_stack, find);

			continue;
		}

		if (t.type == IFNDEF) {
			result = 1;

			Token t2 = lexer_result->tokens[++i];

			char find = 0;

			for (int i = 0; i < state->defines_count; i++) {
				if (strcmp(t2.value, state->defines[i].name) != 0)
					continue;

				find = 1;
			}

			for (int i = 0; i < state->macros_count; i++) {
				if (strcmp(t2.value, state->macros[i].name) != 0)
					continue;

				find = 1;
			}

			stack_push(if_stack, 1 - find);

			continue;
		}


		Token* new_tokens;
		unsigned long new_tokens_count;

		unsigned long offset = 0;

		if (check_macros(state,
		        &lexer_result->tokens[i], lexer_result->tokens_count - i,
		        &new_tokens, &new_tokens_count, &offset)) {
			i += offset;
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


Lexer_result preprocess(void* state, char* filename) {
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
	Lexer_result lexer_result = lexer(data, filename);
	LOG("lexer end: %s\n", filename);

	if (errors_count != 0)
		return lexer_result;

	while (preproc(state, &lexer_result, dn)) {}

	return lexer_result;
}
