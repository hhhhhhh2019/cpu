#ifndef LEXER_H
#define LEXER_H


#include <regex.h>

enum Token_type {
	EOI,
	UNDEFINED,
	HEX_NUMBER,
	BIN_NUMBER,
	DEC_NUMBER,
	COMMA,
	POINT,
	COLON,
	SEMICOLON,
	STRING,
	CHARACTER,
	LBR,
	RBR,
	LSBR,
	RSBR,
	LCBR,
	RCBR,
	INC,
	DEC,
	ARROW,
	ASSIGN_PLUS,
	ASSIGN_MINUS,
	ASSIGN_STAR,
	ASSIGN_SLASH,
	ASSIGN_MOD,
	ASSIGN_LEFT_SHIFT,
	ASSIGN_RIGHT_SHIFT,
	ASSIGN_AND,
	ASSIGN_OR,
	ASSIGN_XOR,
	LEFT_SHIFT,
	RIGHT_SHIFT,
	PLUS,
	MINUS,
	STAR,
	SLASH,
	PERCENT,
	DOUBLE_AMPERSAND,
	DOUBLE_PIPE,
	AMPERSAND,
	PIPE,
	TILDA,
	CARET,
	EXCLAMATION,
	ASSIGN,
	EQUALS,
	NOT_EQUALS,
	LESS,
	LESS_EQUALS,
	MORE,
	MORE_EQUALS,
	CHAR,
	SHORT,
	INT,
	LONG,
	VOID,
	FLOAT,
	DOUBLE,
	SIGNED,
	UNSIGNED,
	STRUCT,
	ENUM,
	UNION,
	EXTERN,
	REGISTER,
	INLINE,
	VOLATILE,
	STATIC,
	RESTRICT,
	AUTO,
	SIZEOF,
	ALIGNAS,
	ALIGNOF,
	CONST,
	CONSTEXPR,
	DO,
	WHILE,
	BREAK,
	CONTINUE,
	FOR,
	IF,
	ELSE,
	TYPEDEF,
	RETURN,
	SWITCH,
	CASE,
	DEFAULT,
	GOTO,
	Start,
	Start1,
	Start11,
	Start111,
	Func1,
	Func11,
	Func_args,
	Func_arg,
	Func_arg1,
	Func_body,
	Func_body1,
	Var,
	Var1,
	Struct_decl,
	Struct,
	Struct1,
	Struct11,
	Struct111,
	Struct_fileds,
	Struct_fileds1,
	Enum_decl,
	Enum,
	Enum1,
	Enum11,
	Enum111,
	Enum_fields,
	Enum_fields1,
	Enum_fields11,
	Union_decl,
	Union,
	Type_can_arr,
	Type_can_arr1,
	Type,
	Type_mod,
	Type_mod1,
	Type_name,
	Number,
	Cond,
	If,
	While,
	For,
	Switch,
	Return,
	Expr,
	A,
	A1,
	Log_or,
	Log_or1,
	Log_and,
	Log_and1,
	Or,
	Or1,
	Xor,
	Xor1,
	And,
	And1,
	C,
	C1,
	B,
	B1,
	S,
	S1,
	H,
	H1,
	J,
	J1,
	T,
	F,
	F1,
	F11,
	Compound,
	Compound1,
	Compound11,
	Func_call,
	Call_args,
	Call_arg,
	Call_arg1,
	Mem_access,
	Point_mem_access,
};


typedef struct {
	enum Token_type type;
	char* value;
	char* filename;
	unsigned long line;
	unsigned long col;
} Token;


typedef struct {
	enum Token_type type;
	char* regex_str;
	regex_t regex;
} Type_regex;


extern Type_regex types_regex[];
extern unsigned long types_regex_count;
extern char** token_type_names;


char lexer_init();


#endif
