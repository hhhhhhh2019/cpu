#include "cc.h"
#include "lexer.h"
#include "token.h"
#include "utils.h"
#include "synt.h"

#include <stdio.h>
#include <stdlib.h>


static char state_start    (Node*, unsigned int*, Compiler_state*);
static char state_var_decl (Node*, unsigned int*, Compiler_state*);
static char state_func_decl(Node*, unsigned int*, Compiler_state*);
static char state_type_decl(Node*, unsigned int*, Compiler_state*);
static char state_expr     (Node*, unsigned int*, Compiler_state*);


char match(unsigned int* offset, Compiler_state* state, unsigned int count, enum Token_type types[]) {
	for (int i = 0; i < count; i++) {
		if (state->lexer_result.tokens[*offset].type == types[i])
			return 1;
	}

	return 0;
};


char require(unsigned int* offset, Compiler_state* state, unsigned int count, enum Token_type types[], Token* result) {
	if (!match(offset, state, count, types)) {
		printf("%s %lu:%lu Expected: ",
				state->lexer_result.tokens[*offset].filename,
				state->lexer_result.tokens[*offset].line,
				state->lexer_result.tokens[*offset].column);
		for (int i = 0; i < count; i++)
			printf("%s ", token_type_names[types[i]]);
		printf("but found %s!\n", token_type_names[state->lexer_result.tokens[*offset].type]);
		return 0;
	}
	*result = state->lexer_result.tokens[(*offset)++];
	return 1;
}


static char state_start(Node* node, unsigned int* offset, Compiler_state* state) {
	return state_expr(node, offset, state);
}


static char state_type(Node* node, unsigned int* offset, Compiler_state* state) {
	
}


static char state_var_decl(Node* node, unsigned int* offset, Compiler_state* state) {
	Node type;
	unsigned int offset_save = *offset;

	if (!state_type_decl(&type, offset, state)) {
		*offset = offset_save;
		return 0;
	}

	
}


static char state_func_decl(Node* node, unsigned int* offset, Compiler_state* state) {

}


static char state_expr(Node* node, unsigned int* offset, Compiler_state* state) {
	return 0;
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
