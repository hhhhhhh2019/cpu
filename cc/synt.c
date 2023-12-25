#include <stdio.h>
#include <synt.h>
#include <cc.h>
#include <lexer.h>

#include <stdlib.h>
#include <assert.h>


static unsigned int offset;


/*void synt(Compiler_state* state) {
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


		* getc(stdin);

		printf("-------\n");

		printf("%s %s\n", token.value, token_type_names[token.type]);

		for (int i = 0; i < stack.values_count; i++) {
			printf("%s ", token_type_names[stack.values[i]]);
		}

		printf("\n"); *

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
}*/



static Node* parse_var(Compiler_state*);
static Node* parse_type(Compiler_state*);
static Node* parse_enum(Compiler_state*);
static Node* parse_union(Compiler_state*);
static Node* parse_struct(Compiler_state*);
static Node* parse_expr  (Compiler_state*);


Token* match(Compiler_state* state, unsigned int count, enum Token_type* types) {
	//assert(offset < state->tokens_count);
	if (offset >= state->tokens_count)
		return NULL;

	enum Token_type type = state->tokens[offset].type;

	for (int i = 0; i < count; i++) {
		if (type == types[i])
			return &state->tokens[offset];
	}

	return NULL;
}


Token* require(Compiler_state* state, unsigned int count, enum Token_type* types, char add) {
	Token* token = match(state, count, types);

	if (add)
		offset++;

	if (token == NULL) {
		if (add)
			offset--;

		Error error = {
			.type = SYNTAX_ERROR,
			.start_token = offset,
			.end_token = offset,
			.text = malloc(1000)
		};

		if (offset < state->tokens_count)
			sprintf(error.text, "%lu:%lu: Ожидалось что-то, а найдено %d \"%s\"",
			    state->tokens[offset].line, state->tokens[offset].col, offset, state->tokens[offset].value);
		else
			sprintf(error.text, "Ожидалось что-то");

		add_error(state, error);
	}

	return token;
}


static Node* parse_expr1a(Compiler_state*);


static Node* parse_expr3(Compiler_state* state) {
	Token* t = require(state, 1, (enum Token_type[]){
	    DEC_NUMBER
	}, 1);

	if (t == NULL)
		return empty_node();

	Node* a = empty_node();
	a->value = *t;

	return a;
}


static Node* parse_expr2a(Compiler_state* state) {
	Token* top = match(state, 1, (enum Token_type[]){
	    STAR
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr3(state));

	Node* op2 = parse_expr2a(state);

	if (op2 != NULL) {
		node_add_child(op2, op);
		return op2;
	}

	return op;
}


static Node* parse_expr2(Compiler_state* state) {
	Node* a = parse_expr3(state);

	Node* op = parse_expr2a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	return op;
}


static Node* parse_expr1a(Compiler_state* state) {
	Token* top = match(state, 1, (enum Token_type[]){
	    PLUS
	    // ASSIGN, ASSIGN_PLUS, ASSIGN_MINUS, ASSIGN_STAR,
	    // ASSIGN_SLASH, ASSIGN_MOD, ASSIGN_LEFT_SHIFT,
	    // ASSIGN_RIGHT_SHIFT, ASSIGN_AND, ASSIGN_OR, ASSIGN_XOR
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr2(state));

	return op;
}


static Node* parse_expr1(Compiler_state* state) {
	Node* a = parse_expr2(state);

	Node* op = parse_expr1a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr1a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr1a(state);
	}

	return op;
}


static Node* parse_expr(Compiler_state* state) {
	Node* root = empty_node();
	root->value.type = Group;

	while (1) {
		Node* node = parse_expr1(state);

		node_add_child(root, node);

		if (match(state, 1, (enum Token_type[]){COMMA}) == NULL)
			break;

		offset++;
	}

	if (root->childs_count == 1) {
		root = root->childs[0];
		free(root->parent->childs);
		free(root->parent);
		root->parent = NULL;
	}

	return root;
}


static Node* parse_struct(Compiler_state* state) {
	if (require(state, 1, (enum Token_type[]){STRUCT}, 1) == NULL)
		return NULL;

	Node* root = empty_node();
	root->value.type = Struct;

	Token* token1 = require(state, 2, (enum Token_type[]){UNDEFINED, LCBR}, 1);

	if (token1 == NULL)
		return root;

	if (token1->type == UNDEFINED) {
		root->value = *token1;
		root->value.type = Struct;

		token1 = match(state, 1, (enum Token_type[]){LCBR});

		if (token1 == NULL)
			return root;

		offset++;
	}

	while (match(state, 1, (enum Token_type[]){RCBR}) == NULL) {
		Node* field = parse_var(state);

		if (require(state, 1, (enum Token_type[]){SEMICOLON}, 1) == NULL)
			break;

		node_add_child(root, field);
	}

	offset++;

	return root;
}


static Node* parse_enum(Compiler_state* state) {
	
}


static Node* parse_union(Compiler_state* state) {
	if (require(state, 1, (enum Token_type[]){UNION}, 1) == NULL)
		return NULL;

	Node* root = empty_node();
	root->value.type = Union;

	Token* token1 = require(state, 2, (enum Token_type[]){UNDEFINED, LCBR}, 1);

	if (token1 == NULL)
		return root;

	if (token1->type == UNDEFINED) {
		root->value = *token1;
		root->value.type = Struct;

		token1 = match(state, 1, (enum Token_type[]){LCBR});

		if (token1 == NULL)
			return root;

		offset++;
	}

	while (match(state, 1, (enum Token_type[]){RCBR}) == NULL) {
		Node* field = parse_var(state);

		if (require(state, 1, (enum Token_type[]){SEMICOLON}, 1) == NULL)
			break;

		node_add_child(root, field);
	}

	offset++;

	return root;
}


static Node* parse_var(Compiler_state* state) {
	Node* root = empty_node();
	root->value.type = Var;

	Node* type = parse_type(state);

	Token* name = require(state, 1, (enum Token_type[]){UNDEFINED}, 1);

	if (name == NULL)
		return root;

	root->value = *name;
	root->value.type = Var;

	node_add_child(root, type);

	return root;
}


static Node* parse_type(Compiler_state* state) {
	Node* root = empty_node();

	Node* node = root;

	Token* token1 = require(state, 18, (enum Token_type[]){
	    VOID, CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, ENUM, STRUCT, UNION,
	    SIGNED, UNSIGNED, EXTERN, STATIC, INLINE, REGISTER, VOLATILE, RESTRICT, AUTO
	}, 1);

	if (token1 == NULL)
		return root;

	while (token1->type == SIGNED || token1->type == UNSIGNED || token1->type == EXTERN ||
	       token1->type == STATIC || token1->type == INLINE || token1->type == REGISTER ||
	       token1->type == VOLATILE || token1->type == RESTRICT || token1->type == AUTO) {
		Node* mod = empty_node();
		mod->value = *token1;

		node_add_child(node, mod);

		node = mod;

		token1 = require(state, 18, (enum Token_type[]){
		    VOID, CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, ENUM, STRUCT, UNION,
		    SIGNED, UNSIGNED, EXTERN, STATIC, INLINE, REGISTER, VOLATILE, RESTRICT, AUTO
		}, 1);
	}

	Node* type = NULL;

	if (token1->type == STRUCT) {
		offset--;
		type = parse_struct(state);
	}
	else if (token1->type == ENUM) {
		offset--;
		type = parse_enum(state);
	}
	else if (token1->type == UNION) {
		offset--;
		type = parse_union(state);
	}
	else {
		type = empty_node();
		type->value = *token1;
	}

	node_add_child(node, type);

	root = root->childs[0];

	free(root->parent->childs);
	free(root->parent);

	return root;
}


static Node* start1(Compiler_state* state) {
	// здесь может быть объявл. структуры, объеденения, перечисления, переменной и функции.
	// переменная и функция начинается с типа, котором может быть структура, объед. или переч.,
	//   поэтому сначала парсим тип.

	Node* root = NULL;

	Node* type = parse_type(state);

	if (type->value.type == Struct || type->value.type == Enum || type->value.type == Union) {
		if (match(state, 1, (enum Token_type[]){SEMICOLON}) != NULL) { // объявл. структ., объед., переч.
			offset++;
			return type;
		}
	}

	root = empty_node();

	Token* name = require(state, 1, (enum Token_type[]){UNDEFINED}, 1);

	if (name == NULL)
		return root;

	root->value = *name;

	Token* token1 = require(state, 3, (enum Token_type[]){
	    ASSIGN, SEMICOLON, LBR
	}, 1);

	if (token1 == NULL)
		return root;

	node_add_child(root, type);

	if (token1->type == SEMICOLON) { // переменная без нач. значения
		root->value.type = Var;
	}

	else if (token1->type == ASSIGN) { // переменная с нач. значением
		root->value.type = Var;
		node_add_child(root, parse_expr(state));

		require(state, 1, (enum Token_type[]){SEMICOLON}, 1);
	}

	else { // функция

	}

	return root;
}


static Node* start(Compiler_state* state) {
	if (offset >= state->tokens_count)
		return NULL;

	Node* root = empty_node();
	root->value.type = Start;

	node_add_child(root, start1(state));
	node_add_child(root, start(state));

	return root;
}


void synt(Compiler_state* state) {
	offset = 0;

	state->AST = start(state);
}



void create_dot_from_node2(Node* node) {
	if (node->value.value == NULL)
		printf("%d [label=\"%s\"]\n", node->id, token_type_names[node->value.type]);
	else
		printf("%d [label=\"%s\n%s\"]\n", node->id, token_type_names[node->value.type], node->value.value);

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
