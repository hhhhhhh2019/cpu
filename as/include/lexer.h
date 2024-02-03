#ifndef LEXER_H
#define LEXER_H


enum Token_type {
	NOT_SET,
	UNDEFINED,
	NEWLINE,
	INSTRUCTION,
	TIMES,
	DATA_ALLOC,
	LABEL,
	STRING,
	CHAR,
	DEC_NUMBER,
	HEX_NUMBER,
	BIN_NUMBER,
	COMMA,
	REGISTER,
	PLUS,
	MINUS,
	STAR,
	SLASH,
	PIPE,
	CARET,
	AMPERSAND,
	TILDA,
	LEFT_PAREN,
	RIGHT_PAREN,
	DEFINE,
	INCLUDE,
	BACK_SLASH,
};


typedef struct {
	char* value;
	enum Token_type type;
	unsigned long line;
	unsigned long column;
} Token;


typedef struct {
	unsigned long tokens_count;
	Token* tokens;
} Lexer_result;



Lexer_result lexer(char* data);


#endif
