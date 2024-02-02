#include "lexer.h"
#include <as.h>

#include <stdlib.h>


static unsigned long offset;
static unsigned long line;
static unsigned long column;


static enum Token_type get_type(char* value) {
	
}


static char next_hex_number(Token* token, char* data) {
	token->type = HEX_NUMBER;
  token->name = calloc(1,1);
	unsigned long offset = 0;

	while (1) {
		char c = *data;

		if (!(('0' <= c && c <= '9') ||
		      ('a' <= c && c <= 'f') ||
		      ('A' <= c && c <= 'F')))
			break;

		token->name = realloc(token->name, ++offset + 1);
		token->name[offset] = 0;
		token->name[offset - 1] = c;
	}

	token->line = line;
	token->column = column;

	column += offset;

	return 1;
}


static char next_bin_number(Token* token, char* data) {
	token->type = BIN_NUMBER;
  token->name = calloc(1,1);
	unsigned long offset = 0;
	
	while (1) {
		char c = *data;

		if (!('0' <= c && c <= '1'))
			break;

		token->name = realloc(token->name, ++offset + 1);
		token->name[offset] = 0;
		token->name[offset - 1] = c;
	}

	token->line = line;
	token->column = column;

	column += offset;

	return 1;
}


static char next_number(Token* token, char* data) {
	if (*(data + 1) == 'x' || *(data + 1) == 'X')
		return next_hex_number(token, data + 2);

	if (*(data + 1) == 'b' || *(data + 1) == 'B')
		return next_bin_number(token, data + 2);


	token->type = DEC_NUMBER;
  token->name = calloc(1,1);
	unsigned long offset = 0;

	
	while (1) {
		char c = *data;

		if (!('0' <= c && c <= '9'))
			break;

		token->name = realloc(token->name, ++offset + 1);
		token->name[offset] = 0;
		token->name[offset - 1] = c;
	}

	token->line = line;
	token->column = column;

	column += offset;

	return 1;
}


static char next_token(Token* token, char* data) {
	token->type = NOT_SET;

	if (*data == 0)
		return 0;

	if (*data == ' ' || *data == '\t')
		return next_token(token, data + 1);

	if ('0' <= *data && *data <= '9')
		return next_number(token, data);

	// if (*data == '#')
	// 	return next_macros(token, data);

	if (*data == '\n') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column;
		line++;
		column = 0;
		return 1;
	}

	if (*data == '+') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '-') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '*') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '/') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '^') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '|') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '&') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '~') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '(') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == ')') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '\\') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == ',') {
		token->name = calloc(2,1);
		token->name[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	
	token->name = calloc(1,1);
	unsigned long offset = 0;

	
	while (1) {
		char c = *data;

		if (c == '+'  || c == '*'  || c == '-' || c == '/' || c == ' ' ||
		    c == '\t' || c == '\\' || c == '|' || c == '^' || c == '&' ||
		    c == '~'  || c == ',')
			break;

		token->name = realloc(token->name, ++offset + 1);
		token->name[offset] = 0;
		token->name[offset - 1] = c;
	}

	token->line = line;
	token->column = column;

	column += offset;


	return 1;
}


Lexer_result lexer(char* data) {
	offset = 0;
	column = 0;
	line   = 0;

	Lexer_result result = {
		.tokens_count = 0,
		.tokens = malloc(0)
	};

	Token token;

	while (next_token(&token, data + offset)) {
		if (token.type == NOT_SET)
			token.type = get_type(token.name);

		result.tokens = realloc(result.tokens, sizeof(Token) * (++result.tokens_count));
		result.tokens[result.tokens_count - 1] = token;
	}

	return result;
}
