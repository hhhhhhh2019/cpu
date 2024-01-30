#include <semant.h>
#include <cc.h>

#include <string.h>
#include <stdio.h>


static void add_var_to_fov(Compiler_state* state, FieldOfView* fov, Node* var) {
	for (int i = 0; i < fov->vars_count; i++) {
		if (strcmp(fov->vars[i].var->value.value, var->value.value) == 0) {
			Error error = {
			    .type = SEMANT_ERROR,
			    .end_token = 0,
			    .start_token = 0,
			    .text = malloc(1000)
			};

			sprintf(error.text, "adwdw");

			add_error(state, error);

			return;
		}
	}

	fov->vars = realloc(fov->vars, sizeof(Token) * (++fov->vars_count));
	fov->vars[fov->vars_count-1] = (Variable){
	    .var = var
	};
}


static void parse_type(Compiler_state* state, Node* node, char func) {
	char _extern = 0;
	char _signed = 0;
	char _unsign = 0;
	char _regist = 0;
	char _inline = 0;
	char _static = 0;
	char _restrc = 0;
	char _volatl = 0;

	while (
	    node->value.type == SIGNED || node->value.type == UNSIGNED ||
	    node->value.type == EXTERN || node->value.type == REGISTER ||
	    node->value.type == INLINE || node->value.type == VOLATILE ||
	    node->value.type == STATIC || node->value.type == RESTRICT) {
		_extern |= node->value.type == EXTERN;
		_signed |= node->value.type == SIGNED;
		_unsign |= node->value.type == UNSIGNED;
		_regist |= node->value.type == REGISTER;
		_inline |= node->value.type == INLINE;
		_static |= node->value.type == STATIC;
		_restrc |= node->value.type == RESTRICT;
		_volatl |= node->value.type == VOLATILE;

		node = node->childs[0];
	}

	while (node->value.type == STAR)
		node = node->childs[0];

	if (_signed && _unsign) {
		Error error = {
		    .type = SEMANT_ERROR,
		    .end_token = 0,
		    .start_token = 0,
		    .text = malloc(1000)
		};

		sprintf(error.text, "signed и unsgined не могут быть вместе");

		add_error(state, error);
	}

	if (func == 0) {
		if (_inline) {
			Error error = {
			    .type = SEMANT_ERROR,
			    .end_token = 0,
			    .start_token = 0,
			    .text = malloc(1000)
			};

			sprintf(error.text, "переменная не может быть inline");

			add_error(state, error);
		}
	}

	if (func == 1) {
		if (_regist) {
			Error error = {
			    .type = SEMANT_ERROR,
			    .end_token = 0,
			    .start_token = 0,
			    .text = malloc(1000)
			};

			sprintf(error.text, "функция не может быть register");

			add_error(state, error);
		}
	}
}


static void parse_func1(Compiler_state* state, Node* node, FieldOfView* parent_fov) {

}


static void parse_func(Compiler_state* state, Node* node, FieldOfView* parent_fov) {
	FieldOfView* fov = malloc(sizeof(FieldOfView));
	fov->parent = parent_fov;
	fov->vars = malloc(0);
	fov->vars_count = 0;

	Node* args = node->childs[1];

	for (int i = 0; i < args->childs_count; i++)
		add_var_to_fov(state, fov, args->childs[i]);

	if (node->childs_count == 2)
		return;

	node = node->childs[2];

	while (1) {
		parse_func1(state, node->childs[0], fov);

		if (node->childs_count == 1)
			break;

		node = node->childs[1];
	}
}


static void parse_node(Compiler_state* state, Node* node, FieldOfView* parent_fov) {
	if (node->value.type == Var || node->value.type == Func_pointer) {
		add_var_to_fov(state, parent_fov, node);
	} else {
		parse_func(state, node, parent_fov);
	}
}


void semant(Compiler_state* state) {
	Node* node = state->AST;

	FieldOfView* fov = malloc(sizeof(FieldOfView));
	fov->parent = NULL;
	fov->vars = malloc(0);
	fov->vars_count = 0;

	while (1) {
		parse_node(state, node->childs[0], fov);

		if (node->childs_count == 1)
			break;

		node = node->childs[1];
	}
}
