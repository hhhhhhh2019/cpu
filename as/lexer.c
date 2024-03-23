#include "lexer.h"
#include "error.h"
#include <as.h>
#include <utils.h>

#include <stdlib.h>
#include <string.h>


static unsigned long offset;
static unsigned long line;
static unsigned long column;


char** alloced_names;
unsigned long alloced_names_size;


char* token_type_names[] = {
	[NOT_SET]     = "NOT_SET",
	[UNDEFINED]   = "UNDEFINED",
	[INSTRUCTION] = "INSTRUCTION",
	[TIMES]       = "TIMES",
	[DATA_ALLOC]  = "DATA_ALLOC",
	[LABEL]       = "LABEL",
	[STRING]      = "STRING",
	[CHAR]        = "CHAR",
	[DEC_NUMBER]  = "DEC_NUMBER",
	[HEX_NUMBER]  = "HEX_NUMBER",
	[BIN_NUMBER]  = "BIN_NUMBER",
	[COMMA] = "COMMA",
	[REGISTER] = "REGISTER",
	[PLUS] = "+",
	[MINUS] = "-",
	[STAR] = "*",
	[SLASH] = "/",
	[PIPE] = "|",
	[CARET] = "^",
	[AMPERSAND] = "&",
	[TILDA] = "~",
	[LEFT_PAREN] = "(",
	[RIGHT_PAREN] = ")",
};


static enum Token_type get_type(char* value) {
	if (strcmp(value, "times") == 0)
		return TIMES;

	if (strcmp(value, "db") == 0 ||
	    strcmp(value, "ds") == 0 ||
	    strcmp(value, "di") == 0 ||
	    strcmp(value, "dl") == 0)
		return DATA_ALLOC;

	if (strcmp(value, "stol")   == 0 ||
	    strcmp(value, "stoi")   == 0 ||
	    strcmp(value, "stos")   == 0 ||
	    strcmp(value, "stob")   == 0 ||
	    strcmp(value, "loal")   == 0 ||
	    strcmp(value, "loai")   == 0 ||
	    strcmp(value, "loas")   == 0 ||
	    strcmp(value, "loab")   == 0 ||
	    strcmp(value, "add")    == 0 ||
	    strcmp(value, "sub")    == 0 ||
	    strcmp(value, "mul")    == 0 ||
	    strcmp(value, "div")    == 0 ||
	    strcmp(value, "smul")   == 0 ||
	    strcmp(value, "sdiv")   == 0 ||
	    strcmp(value, "cmp")    == 0 ||
	    strcmp(value, "adde")   == 0 ||
	    strcmp(value, "addne")  == 0 ||
	    strcmp(value, "addl")   == 0 ||
	    strcmp(value, "addg")   == 0 ||
	    strcmp(value, "addsl")  == 0 ||
	    strcmp(value, "addsg")  == 0 ||
	    strcmp(value, "pushl")  == 0 ||
	    strcmp(value, "pushi")  == 0 ||
	    strcmp(value, "pushs")  == 0 ||
	    strcmp(value, "pushb")  == 0 ||
	    strcmp(value, "popl")   == 0 ||
	    strcmp(value, "popi")   == 0 ||
	    strcmp(value, "pops")   == 0 ||
	    strcmp(value, "popb")   == 0 ||
	    strcmp(value, "call")   == 0 ||
	    strcmp(value, "int")    == 0 ||
	    strcmp(value, "iret")   == 0 ||
	    strcmp(value, "and")    == 0 ||
	    strcmp(value, "or")     == 0 ||
	    strcmp(value, "xor")    == 0 ||
	    strcmp(value, "not")    == 0 ||
	    strcmp(value, "shl")    == 0 ||
	    strcmp(value, "shr")    == 0 ||
	    strcmp(value, "chst")   == 0 ||
	    strcmp(value, "lost")   == 0 ||
	    strcmp(value, "stolk")  == 0 ||
	    strcmp(value, "loalk")  == 0 ||
	    strcmp(value, "chtp")   == 0 ||
	    strcmp(value, "lotp")   == 0 ||
	    strcmp(value, "chflag") == 0 ||
	    strcmp(value, "loflag") == 0 ||
	    strcmp(value, "cint")   == 0)
		return INSTRUCTION;

	if (strcmp(value, "r0")  == 0 ||
			strcmp(value, "r1")  == 0 ||
			strcmp(value, "r2")  == 0 ||
			strcmp(value, "r3")  == 0 ||
			strcmp(value, "r4")  == 0 ||
			strcmp(value, "r5")  == 0 ||
			strcmp(value, "r6")  == 0 ||
			strcmp(value, "r7")  == 0 ||
			strcmp(value, "r8")  == 0 ||
			strcmp(value, "r9")  == 0 ||
			strcmp(value, "r10") == 0 ||
			strcmp(value, "r11") == 0 ||
			strcmp(value, "r12") == 0 ||
			strcmp(value, "r13") == 0 ||
			strcmp(value, "r14") == 0 ||
			strcmp(value, "r15") == 0 ||
			strcmp(value, "sp")  == 0 ||
			strcmp(value, "pc")  == 0)
		return REGISTER;

	if (strcmp(value, "#include") == 0)
		return INCLUDE;

	if (strcmp(value, "#define") == 0)
		return DEFINE;

	if (strcmp(value, "#undef") == 0)
		return DEFINE;

	if (strcmp(value, "#if") == 0)
		return IF;

	if (strcmp(value, "#ifdef") == 0)
		return IFDEF;

	if (strcmp(value, "#ifndef") == 0)
		return IFNDEF;

	if (strcmp(value, "#else") == 0)
		return ELSE;

	if (strcmp(value, "#endif") == 0)
		return ENDIF;

	if (value[strlen(value)-1] == ':') {
		value[strlen(value)-1] = 0;
		return LABEL;
	}

	return UNDEFINED;
}


static char next_hex_number(Token* token, char* data) {
	token->type = HEX_NUMBER;
	token->value = calloc(1,1);
	unsigned long offset = 0;

	while (1) {
		char c = *(data + offset);

		if (!(('0' <= c && c <= '9') ||
		      ('a' <= c && c <= 'f') ||
		      ('A' <= c && c <= 'F')))
			break;

		token->value = realloc(token->value, ++offset + 1);
		token->value[offset] = 0;
		token->value[offset - 1] = c;
	}

	token->line = line;
	token->column = column;

	column += offset;

	return 1;
}


static char next_bin_number(Token* token, char* data) {
	token->type = BIN_NUMBER;
	token->value = calloc(1,1);
	unsigned long offset = 0;
	
	while (1) {
		char c = *(data + offset);

		if (!('0' <= c && c <= '1'))
			break;

		token->value = realloc(token->value, ++offset + 1);
		token->value[offset] = 0;
		token->value[offset - 1] = c;
	}

	token->line = line;
	token->column = column;

	column += offset;

	return 1;
}


static char next_number(Token* token, char* data) {
	if (*(data + 1) == 'x' || *(data + 1) == 'X') {
		offset += 2;
		return next_hex_number(token, data + 2);
	}

	if (*(data + 1) == 'b' || *(data + 1) == 'B') {
		offset += 2;
		return next_bin_number(token, data + 2);
	}


	token->type = DEC_NUMBER;
	token->value = calloc(1,1);
	unsigned long offset = 0;

	
	while (1) {
		char c = *(data + offset);

		if (!('0' <= c && c <= '9'))
			break;

		token->value = realloc(token->value, ++offset + 1);
		token->value[offset] = 0;
		token->value[offset - 1] = c;
	}

	token->line = line;
	token->column = column;

	column += offset;

	return 1;
}


static char next_char(Token* token, char* data) {
	data++;
	offset++;

	token->type = CHAR;
	token->value = calloc(2,1);
	token->value[0] = *data;

	data++;


	if (token->value[0] == '\\') {
		token->value = realloc(token->value, 3);
		token->value[1] = *data;
		token->value[2] = 0;
		data++;
	}

	if (*data != '\'') {
		return 0;
	}

	data++;
	offset++;

	column += offset;

	return 1;
}


static char next_string(Token* token, char* data) {
	data++;
	offset++;

	token->type = STRING;
	token->value = calloc(1,1);
	token->line = line;
	token->column = column;
	unsigned long soffset = 0;


	while ((*data) != 0 && (*data) != '"' && (*data) != '\n') {
		token->value = realloc(token->value, ++soffset + 1);
		token->value[soffset] = 0;
		token->value[soffset - 1] = *data;

		if (*data == '\\') {
			data++;

			token->value = realloc(token->value, ++soffset + 1);
			token->value[soffset] = 0;
			token->value[soffset - 1] = *data;
		}

		data++;
	}

	if ((*data) != '"') {
		add_error((Error){
		    .type = STRING_NOT_CLOSED,
		    .token = *token
		});

		return 0;
	}

	data++;
	offset++;

	column += soffset;

	return 1;
}


static char next_token(Token* token, char* data) {
	token->type = NOT_SET;

	if (*data == 0)
		return 0;

	if (*data == ';') {
		line++;
		column = 0;

		while (*data != '\n' && *data != 0) {
			offset++;
			data++;
		}

		return next_token(token, data);
	}

	if (*data == ' ' || *data == '\t') {
		offset++;
		column++;
		return next_token(token, data + 1);
	}

	if ('0' <= *data && *data <= '9')
		return next_number(token, data);

	if (*data == '\'')
		return next_char(token, data);

	if (*data == '"')
		return next_string(token, data);

	if (*data == '\n') {
		token->type = NEWLINE;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column;
		line++;
		column = 0;
		return 1;
	}

	if (*data == '+') {
		token->type = PLUS;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '-') {
		token->type = MINUS;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '*') {
		token->type = STAR;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '/') {
		token->type = SLASH;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '^') {
		token->type = CARET;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '|') {
		token->type = PIPE;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '&') {
		token->type = AMPERSAND;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '~') {
		token->type = TILDA;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '(') {
		token->type = LEFT_PAREN;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == ')') {
		token->type = RIGHT_PAREN;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == '\\') {
		token->type = BACK_SLASH;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}

	if (*data == ',') {
		token->type = COMMA;
		token->value = calloc(2,1);
		token->value[0] = *data;
		token->line = line;
		token->column = column++;
		return 1;
	}


	token->value = calloc(1,1);
	unsigned long offset = 0;

	while (1) {
		char c = *(data + offset);

		if (c == '+'  || c == '*'  || c == '-'  || c == '/' || c == ' ' ||
		    c == '\t' || c == '\\' || c == '|'  || c == '^' || c == '&' ||
		    c == '~'  || c == ','  || c == '\n' || c == '(' || c == ')')
			break;

		token->value = realloc(token->value, ++offset + 1);
		token->value[offset] = 0;
		token->value[offset - 1] = c;
	}

	token->line = line;
	token->column = column;

	column += offset;

	return 1;
}


Lexer_result lexer(char* data, char* filename) {
	offset = 0;
	column = 0;
	line   = 0;

	unsigned long tokens_count = 0;
	Token* tokens = malloc(0);

	Lexer_result result = {
		.tokens_count = 0,
		.tokens = malloc(0)
	};

	Token token;

	while (next_token(&token, data + offset)) {
		token.filename = filename;
		
		if (token.type != NEWLINE)
			LOG("%ld:%ld %s\n", token.line + 1, token.column + 1, token.value);
		else
			LOG("%ld:%ld NEWLINE\n", token.line + 1, token.column + 1);

		offset += strlen(token.value);

		if (token.type == NOT_SET)
			token.type = get_type(token.value);

		alloced_names = realloc(alloced_names, sizeof(char*) * (++alloced_names_size));
		alloced_names[alloced_names_size - 1] = token.value;

		tokens = realloc(tokens, sizeof(Token) * (++tokens_count));
		tokens[tokens_count - 1] = token;
	}

	
	for (int i = 0; i < tokens_count; i++) {
		if (tokens[i].type == BACK_SLASH && tokens[i + 1].type == NEWLINE) {
			i++;
			continue;
		}

		result.tokens = realloc(result.tokens, sizeof(Token) * (++result.tokens_count));
		result.tokens[result.tokens_count - 1] = tokens[i];
	}


	return result;
}


void free_all_alloced_names() {
	for (int i = 0; i < alloced_names_size; i++) {
		free(alloced_names[i]);
	}
}
