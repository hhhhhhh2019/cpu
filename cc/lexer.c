#include <lexer.h>
#include <cc.h>

#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


unsigned long line;
unsigned long col;

static unsigned long offset;

Token lexer_next_token(Compiler_state*);


char lexer_init() {
	char buf[128];

	for (int i = 0; i < types_regex_count; i++) {
		int err = regcomp(&types_regex[i].regex, types_regex[i].regex_str, REG_EXTENDED);

		if (err != 0) {
			regerror(err, &types_regex[i].regex, buf, sizeof(buf));
			printf("RegEx #%d \"%s\" compilation failed! (%s)\n", i, types_regex[i].regex_str, buf);
			return 1;
		}
	}

	return 0;
}


void lexer(Compiler_state* state) {
	char* undef_str = calloc(1,1);

	unsigned long undef_line = 1;
	unsigned long undef_col = 1;

	line = 1;
	col = 1;
	offset = 0;

	while (offset < state->code_size) {
		if (isspace(state->code[offset])) {
			col++;
			if (state->code[offset] == '\n') {
				line++;
				col = 1;
			}
			offset++;
			continue;
		}

		Token t = lexer_next_token(state);

		if (t.type == UNDEFINED) {
			free(t.value);

			unsigned int size = strlen(undef_str);

			if (size == 0) {
				undef_line = line;
				undef_col  = col;
			}

			undef_str = realloc(undef_str, size+2);

			undef_str[size+0] = state->code[offset];
			undef_str[size+1] = 0;

			col++;

			offset++;
			continue;
		}

		if (strlen(undef_str) != 0) {
			Token nt = {
				.line = undef_line,
				.col = undef_col,
				.type = UNDEFINED,
				.value = undef_str,
				.filename = state->filename
			};

			undef_str = calloc(1,1);

			state->tokens = realloc(state->tokens, sizeof(Token) * (++state->tokens_count));
			state->tokens[state->tokens_count-1] = nt;
		}

		state->tokens = realloc(state->tokens, sizeof(Token) * (++state->tokens_count));
		state->tokens[state->tokens_count-1] = t;
	}

	Token eoi = {
		.line = line,
		.col = col,
		.type = EOI,
		.value = "$",
		.filename = state->filename
	};

	state->tokens = realloc(state->tokens, sizeof(Token) * (++state->tokens_count));
	state->tokens[state->tokens_count-1] = eoi;

	free(undef_str);
}


Token lexer_next_token(Compiler_state* state) {
	if (offset >= state->code_size)
		return (Token){.line=0,.col=0,.type=0,.value=NULL,.filename=NULL};

	regmatch_t match;

	Token t = {
		.line = line,
		.col = col,
		.type = UNDEFINED,
		.value = NULL,
		.filename = state->filename
	};

	int max_size = 0;
	int max_id = -1;

	for (int i = 0; i < types_regex_count; i++) {
		if (regexec(&types_regex[i].regex, state->code + offset, 1, &match, 0) != 0) {
			continue;
		}

		int size = match.rm_eo - match.rm_so;

		if (size <= max_size)
			continue;

		max_size = size;

		t.type = types_regex[i].type;
	}

	if (max_size != 0) {
		t.value = calloc(max_size + 1, 1);
		memcpy(t.value, state->code + offset, max_size);
	}

	offset += max_size;
	col += max_size;

	return t;
}
