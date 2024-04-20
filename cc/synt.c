#include "cc.h"
#include "utils.h"
#include "synt.h"

#include <stdio.h>
#include <stdlib.h>


static char state_start(Node* node, unsigned int* offset, Compiler_state* state);
static char state_expr(Node* node, unsigned int* offset, Compiler_state* state);
static char state_expr_1(Node* node, unsigned int* offset, Compiler_state* state);
static char state_expr_2(Node* node, unsigned int* offset, Compiler_state* state);


static char state_start(Node* node, unsigned int* offset, Compiler_state* state) {
	return state_expr(node, offset, state);
}


static char state_expr_2(Node* node, unsigned int* offset, Compiler_state* state) {
	Node a = {
		.childs_count = 0,
		.childs = malloc(0),
	};

	
	if (state->lexer_result.tokens[*offset].type == LBR) {
		(*offset)++;
		state_expr(&a, offset, state);
		(*offset)++;
		*node = a;
		return 1;
	}

	a.value = state->lexer_result.tokens[(*offset)++];
	*node = a;

	return 1;
}


static char state_expr_1(Node* node, unsigned int* offset, Compiler_state* state) {
	Node a = {
		.childs_count = 0,
		.childs = malloc(0),
	};

	if (!state_expr_2(&a, offset, state)) {
		free(a.childs);
		return 0;
	}

	if (state->lexer_result.tokens[*offset].type != STAR) {
		*node = a;
		return 1;
	}

	Node op = {
		.childs_count = 2,
		.childs = malloc(sizeof(Node) * 2),
		.value = state->lexer_result.tokens[(*offset)++]
	};
	
	Node b = {
		.childs_count = 0,
		.childs = malloc(0),
	};

	if (!state_expr_1(&b, offset, state)) {
		printf("Error\n");
		exit(1);
	}

	op.childs[0] = a;
	op.childs[1] = b;

	*node = op;

	return 1;
}


static char state_expr(Node* node, unsigned int* offset, Compiler_state* state) {
	Node a = {
		.childs_count = 0,
		.childs = malloc(0),
	};

	if (!state_expr_1(&a, offset, state)) {
		free(a.childs);
		return 0;
	}

	if (state->lexer_result.tokens[*offset].type != PLUS) {
		*node = a;
		return 1;
	}

	Node op = {
		.childs_count = 2,
		.childs = malloc(sizeof(Node) * 2),
		.value = state->lexer_result.tokens[(*offset)++]
	};
	
	Node b = {
		.childs_count = 0,
		.childs = malloc(0),
	};

	if (!state_expr(&b, offset, state)) {
		printf("Error\n");
		exit(1);
	}

	op.childs[0] = a;
	op.childs[1] = b;

	*node = op;

	return 1;
}


void synt(Compiler_state* state) {
	unsigned int offset = 0;

	while (offset < state->lexer_result.tokens_count) {
		Node node = {
			.childs = malloc(0),
			.childs_count = 0
		};

		state_start(&node, &offset, state);

		print_node(node, 0);

		break;
	}
}


void print_node(Node node, int offset) {
	for (int i = 0; i < offset; i++)
		putc('\t', stdout);

	LOG("%s %s\n",
			token_type_names[node.value.type],
			node.value.value);

	for (int i = 0; i < node.childs_count; i++)
		print_node(node.childs[i], offset + 1);
}
