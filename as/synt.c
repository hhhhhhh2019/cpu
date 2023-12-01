#include <as.h>
#include <utils.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


char is_terminal(enum Token_type type) {
	for (int i = 0; i < types_regex_count; i++) {
		if (types_regex[i].type == type || type == NEWLINE) {
			return 1;
		}
	}

	return 0;
}


void synt_parse(Parser_state* state) {
	Stack_long stack = {
		malloc(0),
		0
	};

	Stack_long nodes_stack = {
		malloc(0),
		0
	};

	unsigned int token_id = 0;

	Node* root = malloc(sizeof(Node));
	root->value = (Token){
		.type = S,
		.value = NULL,
		.line = 0,
		.col = 0,
		.filepath = NULL
	};
	root->childs = malloc(0);
	root->childs_count = 0;
	root->parent = NULL;

	stack_long_push(&nodes_stack, (long)root);

	stack_long_push(&stack, S);


	while (stack.size > 0) {
		/* for (int i = 0; i < stack.size; i++)
			printf("%s ", token_type_names[stack.values[i]]);
		printf(": "); */

		enum Token_type st = stack_long_pop(&stack);
		Token tok = state->lresult.tokens[token_id];
		Node* node = (Node*)stack_long_pop(&nodes_stack);

		// printf("%s %s\n", tok.value, token_type_names[tok.type]);

		if (st == tok.type) {
			node->value = tok;
			token_id++;
			continue;
		}

		int todo_id = table[st][tok.type];

		if (todo_id == -1) {
			printf("Error\n");
			state->ok = 0;
			return;
		}

		for (int i = 0; i < todo[todo_id][0]; i++) {
			Node* new_node = malloc(sizeof(Node));
			new_node->value = (Token){
				.type = todo[todo_id][i+1],
				.value = NULL,
				.line = 0,
				.col = 0,
				.filepath = NULL
			};
			new_node->parent = node;
			new_node->childs = malloc(0);
			new_node->childs_count = 0;

			node->childs = realloc(node->childs, sizeof(void*) * (++node->childs_count));
			node->childs[node->childs_count-1] = new_node;

			stack_long_push(&stack, todo[todo_id][i+1]);
			stack_long_push(&nodes_stack, (long)new_node);
		}
	}

	free(stack.values);
	free(nodes_stack.values);

	state->sresult.root = root;
}



void create_dot_from_node2(Node* node) {
	if (node->value.value == NULL)
		printf("%d [label=\"%s\"]\n", node->id, token_type_names[node->value.type]);
	else if (node->value.type == NEWLINE)
		printf("%d [label=\"\\\\n\"]\n", node->id);
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


void free_node(Node* node) {
	for (int i = 0; i < node->childs_count; i++)
		free_node(node->childs[i]);

	free(node->childs);

	free(node);
}


void node_pop_child(Node* root, Node* node, char need_free) {
	Node** childs = malloc(0);
	unsigned int childs_count = 0;

	for (int i = 0; i < root->childs_count; i++) {
		if (root->childs[i] == node) {
			if (need_free) free_node(node);
			continue;
		}

		childs = realloc(childs, sizeof(void*) * (++childs_count));
		childs[childs_count-1] = root->childs[i];
	}

	free(root->childs);
	root->childs = childs;
	root->childs_count = childs_count;
}


char collapse_node(Node* node) {
	char terminal = is_terminal(node->value.type);

	if (node->value.type == NEWLINE ||
	    node->value.type == COMMA ||
	    node->value.type == BR_OPEN ||
	    node->value.type == BR_CLOSE) {
		node_pop_child(node->parent, node, 1);
		return 1;
	}

	if (terminal == 1 && node->childs_count == 0)
		return 0;

	Node** childs = malloc(sizeof(void*) * node->childs_count);
	unsigned int childs_count = node->childs_count;

	memcpy(childs, node->childs, sizeof(void*) * node->childs_count);

	for (int i = 0; i < childs_count; i++)
		collapse_node(childs[i]);

	free(childs);


	if (node->childs_count == 0) {
		node_pop_child(node->parent, node, 1);
		return 1;
	}


	if (node->childs_count == 1 && node->value.type != I1 && node->value.type != Data1 && node->value.type != S) {
		Node* child = node->childs[0];

		free(node->childs);

		node->value = child->value;
		node->childs_count = child->childs_count;
		node->childs = child->childs;

		free(child);

		for (int i = 0; i < node->childs_count; i++)
			node->childs[i]->parent = node;

		return 1;
	}


	if (node->value.type == E1 || node->value.type == B1 || node->value.type == T1) {
		Node* child = node->childs[node->childs_count-1];

		node->value = child->value;

		node_pop_child(node, child, 1);

		return 1;
	}


	if (node->value.type == E || node->value.type == B || node->value.type == T) {
		Node* child = node->childs[0];
		Node* child2 = child->childs[0];

		node->value = child->value;
		child->value = child2->value;

		if (child2->childs_count == 0) {
			node_pop_child(child, child2, 1);
		} else if (child2->childs_count == 1) {
			Node* child3 = child2->childs[0];
			free(child2->childs);
			child2->value = child3->value;
			child2->childs = child3->childs;
			child2->childs_count = child3->childs_count;
			free(child3);
			//node_pop_child(child2, child3, 1);
		} else {
			free(child->childs);
			child->childs = child2->childs;
			child->childs_count = child2->childs_count;

			free(child2);
		}

		return 1;
	}


	return 0;
}
