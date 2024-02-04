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
};


static char* messages_russian[] = {
	[PREPROCESS_REDEFINITION] = "переопределение макроса",
	[FILE_NOT_FOUND] = "файл не найден",
	[STRING_NOT_CLOSED] = "строка не закрыта",
	[EXPECT_TOKEN] = "ожидался токен",
};


static char* token_string_english[] = {
	[STRING] = "string",
};


static char* token_string_russian[] = {
	[STRING] = "строка",
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
			ERROR("%s: %lu:%lu: \"%s\": %s: %s\n",
					errors[i].token.filename,
					errors[i].token.line + 1, errors[i].token.column + 1,
					errors[i].token.value,
					messages[errors[i].type],
					token_string[errors[i].excepted_token]);
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
