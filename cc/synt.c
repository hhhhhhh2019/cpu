#include <stdio.h>
#include <synt.h>
#include <cc.h>
#include <lexer.h>

#include <stdlib.h>


static unsigned int offset;


void synt(Compiler_state* state) {
	offset = 0;

	Stack_int stack = {
		.values = malloc(0),
		.values_count = 0
	};

	stack_push(stack, Start);

	while (stack.values_count != 0) {
		Token token = state->tokens[offset];


		getc(stdin);

		printf("-------\n");

		printf("%s %s\n", token.value, token_type_names[token.type]);

		for (int i = 0; i < stack.values_count; i++) {
			printf("%s ", token_type_names[stack.values[i]]);
		}

		printf("\n");

		enum Token_type curr_state = stack_pop(stack);

		if (token.type == curr_state) {
			offset++;
			continue;
		}

		int todo_id = table[curr_state][token.type];

		printf("%d\n", todo_id);

		if (todo_id == -1) {
			Error error = {
				.type = SYNTAX_ERROR,
				.text = malloc(1000)
			};
			sprintf(error.text, "Ошибка на позиции %lu:%lu", token.line, token.col);
			add_error(state, error);
			continue;
		}

		enum Token_type* new_tokens = todo[todo_id];

		for (int i = 0; i < new_tokens[0]; i++)
			stack_push(stack, new_tokens[1 + i]);
	}

	free(stack.values);
}
