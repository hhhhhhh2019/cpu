#include <as.h>
#include <error.h>

#include <stdio.h>
#include <stdlib.h>


static inline Node* empty_node() {
	Node* node = calloc(sizeof(Node),1);
	node->childs = malloc(0);

	return node;
}


static inline void node_add_child(Node* node, Node* child) {
	if (child == NULL)
		return;

	node->childs = realloc(node->childs, sizeof(Node*) * (++node->childs_count));
	node->childs[node->childs_count-1] = child;
	child->parent = node;
}


static unsigned long offset;



Token* match(Compiler_state* state, unsigned int count, enum Token_type* types) {
	//assert(offset < state->lexer_result.tokens_count);
	if (offset >= state->lexer_result.tokens_count)
		return NULL;

	enum Token_type type = state->lexer_result.tokens[offset].type;

	for (int i = 0; i < count; i++) {
		if (type == types[i])
			return &state->lexer_result.tokens[offset];
	}

	return NULL;
}


Token* require(Compiler_state* state, unsigned int count, enum Token_type* types, char hard) {
	Token* token = match(state, count, types);

	offset++;

	if (token == NULL && hard) {
		add_error((Error){
		    .type = UNEXPECTED_TOKEN,
		    .token = state->lexer_result.tokens[offset - 1],
		});
	}

	return token;
}



static Node* parse_expr5_1(Compiler_state*);
static Node* parse_expr5(Compiler_state*);
static Node* parse_expr4_1(Compiler_state*);
static Node* parse_expr4(Compiler_state*);
static Node* parse_expr3_1(Compiler_state*);
static Node* parse_expr3(Compiler_state*);
static Node* parse_expr2_1(Compiler_state*);
static Node* parse_expr2(Compiler_state*);
static Node* parse_expr1_1(Compiler_state*);
static Node* parse_expr1(Compiler_state*);
static Node* parse_expr_1(Compiler_state*);
static Node* parse_expr(Compiler_state*);
static Node* parse_expr_arr(Compiler_state*);



static Node* parse_expr5_1(Compiler_state* state) {
	Token* a = require(state, 6, (enum Token_type[]){
	    BIN_NUMBER, HEX_NUMBER, DEC_NUMBER,
	    CHAR, LEFT_PAREN, UNDEFINED
	}, 1);

	if (a == NULL)
		return empty_node();

	if (a->type == LEFT_PAREN) {
		Node* node = parse_expr(state);
		require(state, 1, (enum Token_type[]){RIGHT_PAREN}, 1);
		return node;
	}

	Node* node = empty_node();
	node->token = *a;

	return node;
}


static Node* parse_expr5(Compiler_state* state) {
	Token* top = match(state, 2, (enum Token_type[]){
	    MINUS, TILDA
	});

	if (top == NULL)
		return parse_expr5_1(state);

	Node* op = empty_node();
	op->token = *top;

	node_add_child(op, parse_expr5_1(state));

	return op;
}


static Node* parse_expr4_1(Compiler_state* state) {
	Token* top = match(state, 2, (enum Token_type[]){
	    STAR, SLASH
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->token = *top;

	node_add_child(op, parse_expr5(state));

	return op;
}


static Node* parse_expr4(Compiler_state* state) {
	Node* a = parse_expr5(state);
	Node* op = parse_expr4_1(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr4_1(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr4_1(state);
	}

	return op;
}


static Node* parse_expr3_1(Compiler_state* state) {
	Token* top = match(state, 2, (enum Token_type[]){
	    PLUS, MINUS
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->token = *top;

	node_add_child(op, parse_expr4(state));

	return op;
}


static Node* parse_expr3(Compiler_state* state) {
	Node* a = parse_expr4(state);
	Node* op = parse_expr3_1(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr3_1(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr3_1(state);
	}

	return op;
}


static Node* parse_expr2_1(Compiler_state* state) {
	Token* top = match(state, 1, (enum Token_type[]){
	    AMPERSAND
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->token = *top;

	node_add_child(op, parse_expr3(state));

	return op;
}


static Node* parse_expr2(Compiler_state* state) {
	Node* a = parse_expr3(state);
	Node* op = parse_expr2_1(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr2_1(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr2_1(state);
	}

	return op;
}


static Node* parse_expr1_1(Compiler_state* state) {
	Token* top = match(state, 1, (enum Token_type[]){
	    CARET
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->token = *top;

	node_add_child(op, parse_expr2(state));

	return op;
}


static Node* parse_expr1(Compiler_state* state) {
	Node* a = parse_expr2(state);
	Node* op = parse_expr1_1(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr1_1(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr1_1(state);
	}

	return op;
}


static Node* parse_expr_1(Compiler_state* state) {
	Token* top = match(state, 1, (enum Token_type[]){
	    PIPE
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->token = *top;

	node_add_child(op, parse_expr2(state));

	return op;
}


static Node* parse_expr(Compiler_state* state) {
	Node* a = parse_expr1(state);
	Node* op = parse_expr_1(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr_1(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr_1(state);
	}

	return op;
}


static Node* parse_expr_arr(Compiler_state* state) {
	Node* root = empty_node();
	root->type = SGROUP;

	while (1) {
		if (match(state, 2, (enum Token_type[]){NEWLINE, DATA_ALLOC}) != NULL)
			break;

		node_add_child(root, parse_expr(state));

		if (state->lexer_result.tokens[offset].type == COMMA)
			offset++;
	}

	return root;
}



static Node* parse_label(Compiler_state* state) {
	Node* node = empty_node();
	node->type = SLABEL;

	node->token = state->lexer_result.tokens[offset++];

	return node;
}


static Node* parse_data_alloc(Compiler_state* state) {
	Node* root = empty_node();
	root->type = SDATA;
	Node* node = root;

	if (state->lexer_result.tokens[offset].type == TIMES) {
		root->token = state->lexer_result.tokens[offset++];
		node_add_child(root, parse_expr(state));

		node_add_child(node, empty_node());
		node = node->childs[node->childs_count-1];
	}

	char first = 1;

	while (state->lexer_result.tokens[offset++].type == DATA_ALLOC) {
		if (first == 0) {
			node_add_child(node, empty_node());
			node = node->childs[node->childs_count-1];
		} else {
			first = 0;
		}

		node->type = SDATA;
		node->token = state->lexer_result.tokens[offset - 1];
		node_add_child(node, parse_expr_arr(state));
	}

	return root;
}


static Node* parse_instruction(Compiler_state* state) {
	Node* root = empty_node();
	root->type = SINSTRUCTION;

	if (state->lexer_result.tokens[offset].type != INSTRUCTION) {
		add_error((Error){
		    .type = EXPECT_TOKEN,
		    .token = state->lexer_result.tokens[offset],
		    .excepted_token = INSTRUCTION,
		});
		return NULL;
	}

	root->token = state->lexer_result.tokens[offset++];

	while (1) {
		if (state->lexer_result.tokens[offset].type == NEWLINE)
			break;

		if (state->lexer_result.tokens[offset].type == REGISTER) {
			Node* arg = empty_node();
			arg->token = state->lexer_result.tokens[offset++];
			node_add_child(root, arg);
		}

		else {
			node_add_child(root, parse_expr(state));
		}

		if (state->lexer_result.tokens[offset].type == COMMA)
			offset++;
	}

	return root;
}


static Node* parse_line(Compiler_state* state) {
	if (state->lexer_result.tokens[offset].type == NEWLINE)
		return NULL;

	if (state->lexer_result.tokens[offset].type == LABEL)
		return parse_label(state);

	if (state->lexer_result.tokens[offset].type == TIMES ||
	    state->lexer_result.tokens[offset].type == DATA_ALLOC)
		return parse_data_alloc(state);

	if (state->lexer_result.tokens[offset].type == INSTRUCTION)
		return parse_instruction(state);


	add_error((Error){
	    .type = UNEXPECTED_TOKEN,
	    .token = state->lexer_result.tokens[offset++],
	});

	return NULL;
}


void synt(Compiler_state* state) {
	offset = 0;

	for (; offset < state->lexer_result.tokens_count;) {
		Node* node = parse_line(state);

		if (node != NULL) {
			state->synt_result.nodes = realloc(state->synt_result.nodes,
			    sizeof(Node*) * (++state->synt_result.nodes_count));
			state->synt_result.nodes[state->synt_result.nodes_count - 1] = node;
		}

		while (
		    offset < state->lexer_result.tokens_count &&
		    state->lexer_result.tokens[offset].type == NEWLINE) {offset++;}
	}
}



void create_dot_from_node2(Node* node) {
	if (node->token.value == NULL)
		printf("%lu [label=\"%s\"]\n", node->id, token_type_names[node->token.type]);
	else
		printf("%lu [label=\"%s\n%s\"]\n", node->id, token_type_names[node->token.type], node->token.value);

	printf("%lu -> {", node->id);

	for (int i = 0; i < node->childs_count; i++) {
		printf("%lu ", node->childs[i]->id);
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
