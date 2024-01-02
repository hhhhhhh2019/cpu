#include <semant.h>
#include <cc.h>



static void parse_node(Compiler_state* state, Node* node, FieldOfView* parent_fov) {
	if (node->value.type == Var) {
		
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
