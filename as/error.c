#include "lexer.h"
#include <error.h>
#include <utils.h>

#include <stdlib.h>
#include <string.h>


unsigned int errors_count = 0;
Error* errors = NULL;


static char* messages_english[] = {
	[PREPROCESS_REDEFINITION] = "macro redefinition",
	[FILE_NOT_FOUND] = "file not found",
	[STRING_NOT_CLOSED] = "string not closed",
	[EXPECT_TOKEN] = "expected token",
	[UNEXPECTED_TOKEN] = "unexpected token",
	[LABEL_REDEFINE] = "label redefinition",
	[INSTRUCTION_NOT_FOUND] = "instruction not found",
	[INSTRUCTION_ARG_ERROR] = "instruction argument error",
	[LABEL_NOT_FOUND] = "label not defined",
};


static char* messages_russian[] = {
	[PREPROCESS_REDEFINITION] = "переопределение макроса",
	[FILE_NOT_FOUND] = "файл не найден",
	[STRING_NOT_CLOSED] = "строка не закрыта",
	[EXPECT_TOKEN] = "ожидался токен",
	[UNEXPECTED_TOKEN] = "непредвиденный токен",
	[LABEL_REDEFINE] = "повторное определение метки",
	[INSTRUCTION_NOT_FOUND] = "инструкция не найдена",
	[INSTRUCTION_ARG_ERROR] = "неправильный аргумент",
	[LABEL_NOT_FOUND] = "метка не определена",
};


static char* token_string_english[] = {
	[STRING] = "string",
	[LEFT_PAREN] = "\"(\"",
	[RIGHT_PAREN] = "\")\"",
	[REGISTER] = "register",
	[DEC_NUMBER] = "number",
	[BIN_NUMBER] = "number",
	[HEX_NUMBER] = "number",
};


static char* token_string_russian[] = {
	[STRING] = "строка",
	[LEFT_PAREN] = "\"(\"",
	[RIGHT_PAREN] = "\")\"",
	[REGISTER] = "регистр",
	[DEC_NUMBER] = "число",
	[HEX_NUMBER] = "число",
	[BIN_NUMBER] = "число",
};


static char** messages;
static char** token_string;


void add_error(Error error) {
	errors = realloc(errors, sizeof(Error) * (++errors_count));
	errors[errors_count - 1] = error;
}


void print_errors() {
	char* lang = getenv("LANG");

	if (strcmp(lang, "ru_RU.UTF-8") == 0) {
		messages = messages_russian;
		token_string = token_string_russian;
	} else {
		messages = messages_english;
		token_string = token_string_english;
	}


	if (errors_count == 0)
		return;

	for (int i = 0; i < errors_count; i++) {
		if (errors[i].type == EXPECT_TOKEN) {
			int msg_len =
			    strlen(errors[i].token.filename) + 2 +
			    dec_len(errors[i].token.line + 1) + 1 + dec_len(errors[i].token.column + 1) + 2 +
			    1 + strlen(errors[i].token.value) + 3 +
			    strlen(messages[errors[i].type]) + 1;

			for (int j = 0; j < errors[i].excepted_tokens_count; j++) {
				if (j == 0)
					msg_len += 1 + strlen(token_string[errors[i].excepted_tokens[j]]);
				else
					msg_len += 2 + strlen(token_string[errors[i].excepted_tokens[j]]);
			}

			char* msg = malloc(msg_len + 1);

			sprintf(msg, "%s: %lu:%lu: \"%s\": %s:",
			    errors[i].token.filename,
			    errors[i].token.line + 1, errors[i].token.column + 1,
			    errors[i].token.value,
			    messages[errors[i].type]);

			char printed[TOKENS_COUNT];

			for (int j = 0; j < TOKENS_COUNT; j++)
				printed[j] = 0;

			for (int j = 0; j < errors[i].excepted_tokens_count; j++) {
				if (printed[errors[i].excepted_tokens[j]] == 1)
					continue;

				if (j == 0)
					sprintf(msg, "%s %s", msg, token_string[errors[i].excepted_tokens[j]]);
				else
					sprintf(msg, "%s, %s", msg, token_string[errors[i].excepted_tokens[j]]);

				printed[errors[i].excepted_tokens[j]] = 1;
			}

			ERROR("%s\n", msg);

			// ERROR("%s: %lu:%lu: \"%s\": %s: %s\n",
			// 		errors[i].token.filename,
			// 		errors[i].token.line + 1, errors[i].token.column + 1,
			// 		errors[i].token.value,
			// 		messages[errors[i].type],
			// 		token_string[errors[i].excepted_token]);
		} else {
			ERROR("%s: %lu:%lu: \"%s\": %s\n",
					errors[i].token.filename,
					errors[i].token.line + 1, errors[i].token.column + 1,
					errors[i].token.value,
					messages[errors[i].type]);
		}
	}

	exit(1);
}
