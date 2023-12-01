#include <as.h>
#include <lexer.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


Type_regex_t types_regex[] = {
	{HEX_NUMBER,  "^0x[0-9a-fA-F]+"},
	{DEC_NUMBER,  "^[0-9]+"},
	{BIN_NUMBER,  "^0b[0-1]+"},

	{CHARACTER,   "^'.'"},

	{REGISTER,    "^(r0|r1|r2|r3|r4|r5|r6|r7|r8|r9|r10|r11|r12|r13|r14|r15|pc|sp)"},
	{DB,          "^db"},
	{DS,          "^ds"},
	{DI,          "^di"},
	{DL,          "^dl"},
	{TIMES,       "^times"},
	//{MOV,         "^mov"},

	{STOL,        "^stol"},
	{STOI,        "^stoi"},
	{STOS,        "^stos"},
	{STOB,        "^stob"},
	{LOAL,        "^loal"},
	{LOAI,        "^loai"},
	{LOAS,        "^loas"},
	{LOAB,        "^loab"},

	{ADDE,        "^adde"},
	{ADDNE,       "^addne"},
	{ADDL,        "^addl"},
	{ADDG,        "^addg"},
	{ADDO,        "^addo"},
	{ADDNO,       "^addno"},

	{ADD,         "^add"},
	{SUB,         "^sub"},
	{MUL,         "^mul"},
	{DIV,         "^div"},
	{SMUL,        "^smul"},
	{SDIV,        "^sdiv"},
	{CMP,         "^cmp"},

	{PUSHL,       "^pushl"},
	{PUSHI,       "^pushi"},
	{PUSHS,       "^pushs"},
	{PUSHB,       "^pushb"},
	{POPL,        "^popl"},
	{POPI,        "^popi"},
	{POPS,        "^pops"},
	{POPB,        "^popb"},

	{CALL,        "^call"},
	{RET,         "^ret"},
	{INT,         "^int"},
	{IRET,        "^iret"},
	{CINT,        "^cint"},

	{AND,         "^and"},
	{OR,          "^or"},
	{XOR,         "^xor"},
	{NOT,         "^not"},
	{SHL,         "^shl"},
	{SHR,         "^shr"},

	{CHST,        "^chst"},
	{LOST,        "^lost"},
	{STOLK,       "^stolk"},
	{LOALK,       "^loalk"},
	{CHTP,        "^chtp"},
	{LOTP,        "^lotp"},
	{CHFLAG,      "^chflag"},
	{LOFLAG,      "^loflag"},

	{PLUS,        "^\\+"},
	{MINUS,       "^\\-"},
	{MULTIPLE,    "^\\*"},
	{DIVIDE,      "^\\/"},
	{BR_OPEN,     "^\\("},
	{BR_CLOSE,    "^\\)"},

	{COMMA,       "^,"},
	{COLON,       "^:"},

	{CURRENTADDR, "^\\$"},
	{LASTADDR,    "^\\$\\$"},

	{OFFSET,      "\\.offset"},

	{UNDEFINED,   "^[a-zA-Z\\.]+[a-zA-Z0-9\\.]*"},
};

unsigned int types_regex_count = sizeof(types_regex) / sizeof(Type_regex_t);


char lexer_init() {
	for (int i = 0; i < types_regex_count; i++) {
		if (regcomp(&types_regex[i].regex, types_regex[i].regex_str, REG_EXTENDED)) {
			printf("RegEx #%d \"%s\" compilation failed!\n", i, types_regex[i].regex_str);
			return 1;
		}
	}

	return 0;
}



unsigned long loffset;
unsigned long col;
unsigned long line;


void lexer_parse(Parser_state* state) {
	state->lresult.real_tokens = malloc(0);
	state->lresult.real_tokens_count = 0;

	state->lresult.tokens = malloc(0);
	state->lresult.tokens_count = 0;

	loffset = 0;
	col = 0;
	line = 1;


	char is_comment = 0;
	char is_string = 0;

	char* string = NULL;


	while (loffset < state->filedata_size) {
		Token t;

		char c = state->filedata[loffset++];

		col++;


		if (c == ';') {
			is_comment = 1;
		}


		if (c == '\n') {
			if (is_string) {
				printf("%s %lu:%lu Ожидалось '\"'!\n", state->filename, line,col);
				state->ok = 0;
				return;
			}


			t = (Token){
				.col = col,
				.line = line,
				.filepath = state->filepath,
				.type = NEWLINE,
				.value = "\n"
			};
			
			state->lresult.tokens = realloc(
			    state->lresult.tokens,
			    sizeof(Token) * (++state->lresult.tokens_count));
			state->lresult.tokens[state->lresult.tokens_count-1] = t;

			line++;
			col = 0;

			is_comment = 0;

			continue;
		}



		if ((isspace(c) && !is_string) || is_comment)
			continue;

		if (c == '"') {
			if (is_string) {
				t = (Token){
					.col = col - strlen(string) - 1,
					.line = line,
					.filepath = state->filepath,
					.type = STRING,
					.value = string
				};

				state->lresult.real_tokens = realloc(
			    state->lresult.real_tokens,
			    sizeof(Token) * (++state->lresult.real_tokens_count));
				state->lresult.real_tokens[state->lresult.real_tokens_count-1] = t;

				state->lresult.tokens = realloc(
				    state->lresult.tokens,
				    sizeof(Token) * (++state->lresult.tokens_count));
				state->lresult.tokens[state->lresult.tokens_count-1] = t;
			}

			else {
				string = calloc(1,1);
			}

			is_string = 1 - is_string;

			continue;
		}


		if (is_string) {
			int len = strlen(string);

			string = realloc(string, len+2);
			string[len+1] = 0;
			string[len] = c;

			continue;
		}


		loffset--;

		t = lexer_next_token(state);
		
		state->lresult.real_tokens = realloc(
		    state->lresult.real_tokens,
		    sizeof(Token) * (++state->lresult.real_tokens_count));
		state->lresult.real_tokens[state->lresult.real_tokens_count-1] = t;

		state->lresult.tokens = realloc(
		    state->lresult.tokens,
		    sizeof(Token) * (++state->lresult.tokens_count));
		state->lresult.tokens[state->lresult.tokens_count-1] = t;
	}


	state->lresult.tokens = realloc(
	    state->lresult.tokens,
	    sizeof(Token) * (++state->lresult.tokens_count));
	state->lresult.tokens[state->lresult.tokens_count-1] = (Token){
		.col = 0,
		.line = 0,
		.value = "\0",
		.type = EOI
	};
}


Token lexer_next_token(Parser_state* state) {
	if (loffset >= state->filedata_size)
		return (Token){0,NULL,0,0};

	Token t = {
		.col = col,
		.line = line,
		.filepath = state->filepath,
	};

	regmatch_t match;

	for (int i = 0; i < types_regex_count; i++) {
		if (regexec(&types_regex[i].regex, state->filedata + loffset, 1, &match, 0) != 0)
			continue;

		t.type = types_regex[i].type;
		break;
	}

	t.value = calloc(match.rm_eo - match.rm_so + 1, 1);
	memcpy(t.value, state->filedata + match.rm_so + loffset, match.rm_eo - match.rm_so);

	loffset += match.rm_eo - match.rm_so;
	col += match.rm_eo - match.rm_so - 1;

	return t;
}
