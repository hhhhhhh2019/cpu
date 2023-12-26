#include <stdio.h>
#include <synt.h>
#include <cc.h>
#include <lexer.h>

#include <stdlib.h>
#include <assert.h>


static unsigned int offset;



static Node* parse_var(Compiler_state*);
static Node* parse_type(Compiler_state*);
static Node* parse_enum(Compiler_state*);
static Node* parse_union(Compiler_state*);
static Node* parse_struct(Compiler_state*);
static Node* parse_expr  (Compiler_state*);
static Node*   parse_expr1(Compiler_state*);
static Node*  parse_expr1a(Compiler_state*);
static Node*   parse_expr2(Compiler_state*);
static Node*  parse_expr2a(Compiler_state*);
static Node*   parse_expr3(Compiler_state*);
static Node*  parse_expr3a(Compiler_state*);
static Node*   parse_expr4(Compiler_state*);
static Node*  parse_expr4a(Compiler_state*);
static Node*   parse_expr5(Compiler_state*);
static Node*  parse_expr5a(Compiler_state*);
static Node*   parse_expr6(Compiler_state*);
static Node*  parse_expr6a(Compiler_state*);
static Node*   parse_expr7(Compiler_state*);
static Node*  parse_expr7a(Compiler_state*);
static Node*   parse_expr8(Compiler_state*);
static Node*  parse_expr8a(Compiler_state*);
static Node*   parse_expr9(Compiler_state*);
static Node*  parse_expr9a(Compiler_state*);
static Node*  parse_expr10(Compiler_state*);
static Node* parse_expr10a(Compiler_state*);
static Node*  parse_expr11(Compiler_state*);
static Node* parse_expr11a(Compiler_state*);
static Node*  parse_expr12(Compiler_state*);
static Node*  parse_expr13(Compiler_state*);


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



static Node* parse_expr13(Compiler_state* state) {
	// TODO: make function readable

	Token* t1 = require(state, 8, (enum Token_type[]){
	    UNDEFINED, DEC_NUMBER, BIN_NUMBER, HEX_NUMBER,
	    FLOAT, DOUBLE, LCBR, LBR
	}, 1);

	if (t1 == NULL)
		return empty_node();


	if (t1->type == LBR) {
		Node* node = parse_expr1(state);
		require(state, 1, (enum Token_type[]){RBR}, 1);
		return node;
	}


	if (t1->type == DEC_NUMBER || t1->type == BIN_NUMBER ||
	    t1->type == HEX_NUMBER || t1->type == FLOAT || t1->type == DOUBLE) {
		Node* a = empty_node();
		a->value = *t1;
		return a;
	}


	if (t1->type == LCBR) {
		Node* a = parse_expr(state);

		require(state, 1, (enum Token_type[]){RCBR}, 1);

		return a;
	}


	Node* a = empty_node();
	a->value = *t1;

	Token* top = match(state, 6, (enum Token_type[]){
	    INC, DEC, LBR, LSBR, POINT, ARROW
	});

	if (top == NULL)
		return a;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	if (top->type == INC || top->type == DEC) {
		if (top->type == INC)
			a->value.type = INC_POST;
		else
			a->value.type = DEC_POST;

		node_add_child(op, a);

		return op;
	}


	if (top->type == LBR) { // function call
		free(op->childs);
		free(op);

		op = a;

		a = parse_expr(state);

		require(state, 1, (enum Token_type[]){RBR}, 1);

		node_add_child(op, a);

		return op;
	}


	if (top->type == POINT || top->type == ARROW) {
		Token* t2 = require(state, 1, (enum Token_type[]){UNDEFINED}, 1);

		if (t2 == NULL)
			return op;

		Node* b = empty_node();
		b->value = *t2;

		node_add_child(op, a);
		node_add_child(op, b);

		top = match(state, 2, (enum Token_type[]){POINT, ARROW});

		while (top != NULL) {
			offset++;

			Node* op2 = empty_node();
			op2->value = *top;

			node_add_child(op2, op);

			op = op2;

			t2 = require(state, 1, (enum Token_type[]){UNDEFINED}, 1);

			if (t2 == NULL)
				return op;

			Node* b = empty_node();
			b->value = *t2;
	
			node_add_child(op, b);

			top = match(state, 2, (enum Token_type[]){POINT, ARROW});
		}

		return op;
	}


	// a[expr][expr]...

	op->value.type = ARRAY_INDEX;

	node_add_child(op, a);
	node_add_child(op, parse_expr1(state));

	if (require(state, 1, (enum Token_type[]){RSBR}, 1) == NULL)
		return op;

	top = match(state, 1, (enum Token_type[]){LSBR});

	while (top != NULL) {
		offset++;

		Node* op2 = empty_node();
		op2->value = *top;
		op2->value.type = ARRAY_INDEX;

		node_add_child(op2, op);
		node_add_child(op2, parse_expr1(state));

		op = op2;

		if (require(state, 1, (enum Token_type[]){RSBR}, 1) == NULL)
			return op;
	
		top = match(state, 1, (enum Token_type[]){LSBR});
	}

	return op;
}


static Node* parse_expr12(Compiler_state* state) {
	Token* top = match(state, 8, (enum Token_type[]){
	    INC, DEC, PLUS, MINUS, STAR, AMPERSAND, SIZEOF, LBR
	});

	if (top == NULL)
		return parse_expr13(state);

	offset++;

	if (top->type == LBR) { // type cast or (Expr)
		Token* token1 = match(state, 18, (enum Token_type[]){
		    VOID, CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, ENUM, STRUCT, UNION,
		    SIGNED, UNSIGNED, EXTERN, STATIC, INLINE, REGISTER, VOLATILE, RESTRICT, AUTO
		});

		if (token1 == NULL) {
			offset--;
			return parse_expr13(state);
		}

		Node* op = parse_type(state);

		if (require(state, 1, (enum Token_type[]){RBR}, 1) == NULL)
			return NULL;

		node_add_child(op, parse_expr13(state));

		return op;
	}

	Node* op = empty_node();
	op->value = *top;

	if (top->type == SIZEOF) {
		if (require(state, 1, (enum Token_type[]){LBR}, 1) == NULL)
			return NULL;

		node_add_child(op, parse_expr13(state));

		if (require(state, 1, (enum Token_type[]){RBR}, 1) == NULL)
			return NULL;

		return op;
	}

	node_add_child(op, parse_expr13(state));

	return op;
}


static Node* parse_expr11a(Compiler_state* state) {
	Token* top = match(state, 3, (enum Token_type[]){
	    STAR, SLASH, PERCENT
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr12(state));

	return op;
}

static Node* parse_expr11(Compiler_state* state) {
	Node* a = parse_expr12(state);

	Node* op = parse_expr11a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr11a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr11a(state);
	}

	return op;
}


static Node* parse_expr10a(Compiler_state* state) {
	Token* top = match(state, 2, (enum Token_type[]){
	    PLUS, MINUS
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr11(state));

	return op;
}

static Node* parse_expr10(Compiler_state* state) {
	Node* a = parse_expr11(state);

	Node* op = parse_expr10a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr10a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr10a(state);
	}

	return op;
}


static Node* parse_expr9a(Compiler_state* state) {
	Token* top = match(state, 2, (enum Token_type[]){
	    LEFT_SHIFT, RIGHT_SHIFT
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr10(state));

	return op;
}

static Node* parse_expr9(Compiler_state* state) {
	Node* a = parse_expr10(state);

	Node* op = parse_expr9a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr9a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr9a(state);
	}

	return op;
}


static Node* parse_expr8a(Compiler_state* state) {
	Token* top = match(state, 4, (enum Token_type[]){
	    LESS, LESS_EQUALS, MORE, MORE_EQUALS
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr9(state));

	return op;
}

static Node* parse_expr8(Compiler_state* state) {
	Node* a = parse_expr9(state);

	Node* op = parse_expr8a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr8a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr8a(state);
	}

	return op;
}

static Node* parse_expr7a(Compiler_state* state) {
	Token* top = match(state, 2, (enum Token_type[]){
	    EQUALS, NOT_EQUALS
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr8(state));

	return op;
}

static Node* parse_expr7(Compiler_state* state) {
	Node* a = parse_expr8(state);

	Node* op = parse_expr7a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr7a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr7a(state);
	}

	return op;
}


static Node* parse_expr6a(Compiler_state* state) {
	Token* top = match(state, 1, (enum Token_type[]){
	    AMPERSAND
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr7(state));

	return op;
}

static Node* parse_expr6(Compiler_state* state) {
	Node* a = parse_expr7(state);

	Node* op = parse_expr6a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr6a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr6a(state);
	}

	return op;
}


static Node* parse_expr5a(Compiler_state* state) {
	Token* top = match(state, 1, (enum Token_type[]){
	    CARET
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr6(state));

	return op;
}

static Node* parse_expr5(Compiler_state* state) {
	Node* a = parse_expr6(state);

	Node* op = parse_expr5a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr5a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr5a(state);
	}

	return op;
}


static Node* parse_expr4a(Compiler_state* state) {
	Token* top = match(state, 1, (enum Token_type[]){
	    PIPE
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr5(state));

	return op;
}

static Node* parse_expr4(Compiler_state* state) {
	Node* a = parse_expr5(state);

	Node* op = parse_expr4a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr4a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr4a(state);
	}

	return op;
}


static Node* parse_expr3a(Compiler_state* state) {
	Token* top = match(state, 1, (enum Token_type[]){
	    DOUBLE_AMPERSAND
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr4(state));

	return op;
}

static Node* parse_expr3(Compiler_state* state) {
	Node* a = parse_expr4(state);

	Node* op = parse_expr3a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr3a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr3a(state);
	}

	return op;
}


static Node* parse_expr2a(Compiler_state* state) {
	Token* top = match(state, 1, (enum Token_type[]){
	    DOUBLE_PIPE
	});

	if (top == NULL)
		return NULL;

	offset++;

	Node* op = empty_node();
	op->value = *top;

	node_add_child(op, parse_expr3(state));

	return op;
}

static Node* parse_expr2(Compiler_state* state) {
	Node* a = parse_expr3(state);

	Node* op = parse_expr2a(state);

	if (op == NULL)
		return a;

	node_add_child(op, a);

	Node* op2 = parse_expr2a(state);

	while (op2 != NULL) {
		node_add_child(op2, op);
		op = op2;
		op2 = parse_expr2a(state);
	}

	return op;
}


static Node* parse_expr1a(Compiler_state* state) {
	Token* top = match(state, 11, (enum Token_type[]){
	    ASSIGN, ASSIGN_PLUS, ASSIGN_MINUS, ASSIGN_STAR,
	    ASSIGN_SLASH, ASSIGN_MOD, ASSIGN_LEFT_SHIFT,
	    ASSIGN_RIGHT_SHIFT, ASSIGN_AND, ASSIGN_OR, ASSIGN_XOR
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

	if (match(state, 2, (enum Token_type[]){RBR, RCBR}) != NULL)
		return root;

	while (1) {
		Node* node = parse_expr1(state);

		node_add_child(root, node);

		if (match(state, 1, (enum Token_type[]){COMMA}) == NULL)
			break;

		offset++;
	}

	/* if (root->childs_count == 1) {
		root = root->childs[0];
		free(root->parent->childs);
		free(root->parent);
		root->parent = NULL;
	} */

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

	while (match(state, 1, (enum Token_type[]){LSBR}) != NULL) {
		offset++;

		Node* arr = empty_node();
		arr->value.type = ARRAY_INDEX;

		if (match(state, 1, (enum Token_type[]){RSBR}) == NULL)
			node_add_child(arr, parse_expr1(state));

		node_add_child(arr, type);
		type = arr;

		require(state, 1, (enum Token_type[]){RSBR}, 1);
	}

	root->value = *name;
	root->value.type = Var;

	node_add_child(root, type);

	return root;
}


static Node* parse_type(Compiler_state* state) {
	Node* root = empty_node();
	root->value.type = Type;

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

	Token* mod2 = match(state, 2, (enum Token_type[]){STAR, LSBR});

	while (mod2 != NULL) {
		offset++;

		Node* mod = empty_node();
		mod->value = *mod2;

		if (mod2->type == LSBR)
			require(state, 1, (enum Token_type[]){RSBR}, 1);

		node_add_child(node, mod);
		node = mod;

		mod2 = match(state, 2, (enum Token_type[]){STAR, LSBR});
	}

	node_add_child(node, type);

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

	while (match(state, 1, (enum Token_type[]){LSBR}) != NULL) {
		offset++;

		Node* arr = empty_node();
		arr->value.type = ARRAY_INDEX;

		if (match(state, 1, (enum Token_type[]){RSBR}) == NULL)
			node_add_child(arr, parse_expr1(state));

		node_add_child(arr, type);
		type = arr;

		require(state, 1, (enum Token_type[]){RSBR}, 1);
	}

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
		node_add_child(root, parse_expr1(state));

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
