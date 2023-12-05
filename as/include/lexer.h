#ifndef H
#define H


#include <regex.h>


enum Token_type {
	EOI,
	HEX_NUMBER,
	BIN_NUMBER,
	DEC_NUMBER,
	NEWLINE,
	COMMA,
	COLON,
	STRING,
	CHARACTER,
	REGISTER,
	CURRENTADDR,
	LASTADDR,
	DB,
	DS,
	DI,
	DL,
	TIMES,
	ADDE,
	ADDNE,
	ADDL,
	ADDG,
	ADDSL,
	ADDSG,
	ADD,
	SUB,
	MUL,
	DIV,
	SMUL,
	SDIV,
	CMP,
	STOL,
	STOI,
	STOS,
	STOB,
	LOAL,
	LOAI,
	LOAS,
	LOAB,
	PUSHL,
	PUSHI,
	PUSHS,
	PUSHB,
	POPL,
	POPI,
	POPS,
	POPB,
	CALL,
	RET,
	INT,
	IRET,
	CINT,
	AND,
	OR,
	XOR,
	NOT,
	SHL,
	SHR,
	CHST,
	LOST,
	STOLK,
	LOALK,
	CHTP,
	LOTP,
	CHFLAG,
	LOFLAG,
	PLUS,
	MINUS,
	MULTIPLE,
	DIVIDE,
	BR_OPEN,
	BR_CLOSE,
	AMPERSAND,
	PIPE,
	TILDA,
	CARET,
	UNDEFINED,
	OFFSET,
	S,
	S1,
	Label,
	Data,
	Times,
	DataType,
	Data1,
	Data11,
	Offset,
	Instr,
	I1,
	I11,
	I111,
	Instr_name,
	E,
	E1,
	B,
	B1,
	T,
	T1,
	Number,
	F,
	F1,
};


typedef struct Type_regex_t {
	enum Token_type type;
	char* regex_str;
	regex_t regex;
} Type_regex_t;


typedef struct Token {
	enum Token_type type;
	char* value;
	unsigned long line;
	unsigned long col;

	char* filepath;
} Token;


typedef struct Lexer_result {
	unsigned long tokens_count;
	Token* tokens;

	unsigned long real_tokens_count;
	Token* real_tokens; // для отчистки
} Lexer_result;



extern Type_regex_t types_regex[];
extern unsigned int types_regex_count;


struct Parser_state;

char lexer_init();

Token lexer_next_token(struct Parser_state*);


#endif
