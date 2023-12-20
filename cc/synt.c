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


	Stack_long nodes_stack = {
		.values = malloc(0),
		.values_count = 0
	};

	Node* root = malloc(sizeof(Node));
	state->AST = root;
	root->value = (Token){
		.type = S,
		.value = NULL,
		.line = 0,
		.col = 0,
		.filename = NULL
	};
	root->childs = malloc(0);
	root->childs_count = 0;
	root->parent = NULL;

	stack_push(nodes_stack, (long)root);


	while (stack.values_count != 0) {
		Token token = state->tokens[offset];
		Node* node = (Node*)stack_pop(nodes_stack);


		/* getc(stdin);

		printf("-------\n");

		printf("%s %s\n", token.value, token_type_names[token.type]);

		for (int i = 0; i < stack.values_count; i++) {
			printf("%s ", token_type_names[stack.values[i]]);
		}

		printf("\n"); */

		enum Token_type curr_state = stack_pop(stack);

		if (token.type == curr_state) {
			offset++;
			continue;
		}

		int todo_id = table[curr_state][token.type];

		// printf("%d\n", todo_id);

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

		for (int i = 0; i < new_tokens[0]; i++) {
			stack_push(stack, new_tokens[1 + i]);

			Node* new_node = malloc(sizeof(Node));
			new_node->value = (Token){
				.type = todo[todo_id][i+1],
				.value = NULL,
				.line = 0,
				.col = 0,
				.filename = NULL
			};
			new_node->parent = node;
			new_node->childs = malloc(0);
			new_node->childs_count = 0;

			node->childs = realloc(node->childs, sizeof(void*) * (++node->childs_count));
			node->childs[node->childs_count-1] = new_node;

			stack_push(nodes_stack, (long)new_node);
		}
	}

	free(stack.values);
	free(nodes_stack.values);
}



void create_dot_from_node2(Node* node) {
	if (node->value.value == NULL)
		printf("%d [label=\"%s\"]\n", node->id, token_type_names[node->value.type]);
	else
		printf("%d [label=\"%s\"]\n", node->id, node->value.value);

	printf("%d -> {", node->id);

	for (int i = 0; i < node->childs_count; i++) {
		printf("%d ", node->childs[i]->id);
	}

	printf("}\n");

	for (int i = 0; i < node->childs_count; i++)
		create_dot_from_node2(node->childs[i]);
}


int index_nodes(Node* node, int id) {
	node->id = id++;

	for (int i = 0; i < node->childs_count; i++)
		id = index_nodes(node->childs[i], id);

	return id;
}


void create_dot_from_node(Node* node) {
	printf("digraph 1 {\n");

	index_nodes(node, 0);

	create_dot_from_node2(node);

	printf("}\n");
}
