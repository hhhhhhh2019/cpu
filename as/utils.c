#include <utils.h>
#include <error.h>
#include <lexer.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


long dec2l(char* s) {
	long result = 0;

	while (*s) {
		result *= 10;
		result += *s - '0';
		s++;
	}

	return result;
}


long hex2l(char* s) {
	long result = 0;

	while (*s) {
		result *= 16;
		if ('0' <= *s && *s <= '9')
			result += *s - '0';
		else
			result += *s - 'a' + 10;
		s++;
	}

	return result;
}


long bin2l(char* s) {
	long result = 0;

	while (*s) {
		result *= 2;
		result += *s - '0';
		s++;
	}

	return result;
}


unsigned long get_next_offset(Compiler_state* state, unsigned long id) {
	if (id >= state->synt_result.nodes_count)
		return state->synt_result.nodes[state->synt_result.nodes_count - 1]->offset;

	while (
	    state->synt_result.nodes[id]->token.type == LABEL &&
	    id < state->synt_result.nodes_count)
		id++;

	return state->synt_result.nodes[id]->offset;
}


unsigned long get_prev_offset(Compiler_state* state, unsigned long id) {
	if (id == 0 || id == -1)
		return get_next_offset(state, id);

	while (state->synt_result.nodes[id]->token.type == LABEL) {
		if (id == 0)
			return 0;

		id--;
	}

	return state->synt_result.nodes[id]->offset;
}


long solve_expression(
    Compiler_state* state, char allow_labels,
    char* last_label, Node* node, unsigned long id) {
	if (node->token.type == DEC_NUMBER)
		return dec2l(node->token.value);
	if (node->token.type == HEX_NUMBER)
		return hex2l(node->token.value);
	if (node->token.type == BIN_NUMBER)
		return bin2l(node->token.value);
	if (node->token.type == CHAR) {
		if (node->token.value[0] == '\\') {
			if (node->token.value[1] == 'a')
				return '\a';
			if (node->token.value[1] == 'b')
				return '\b';
			if (node->token.value[1] == 'f')
				return '\f';
			if (node->token.value[1] == 'n')
				return '\n';
			if (node->token.value[1] == 'r')
				return '\r';
			if (node->token.value[1] == 't')
				return '\t';
			if (node->token.value[1] == 'v')
				return '\v';
			if (node->token.value[1] == '\\')
				return '\\';
			if (node->token.value[1] == '\'')
				return '\'';
			if (node->token.value[1] == '"')
				return '\"';
			if (node->token.value[1] == '?')
				return '\?';
			if (node->token.value[1] == '0')
				return '\0';
			if (node->token.value[1] == 'e')
				return '\e';
		}
		return node->token.value[0];
	}

	if (node->token.type == UNDEFINED) {
		if (allow_labels == 0) {
			return 0;
		}

		char is_last_offset = 1;

		unsigned long deep = 0;

		char* s = node->token.value;

		while (*s != 0) {
			if (*s != '$') {
				is_last_offset = 0;
				break;
			}

			s++;
			deep++;
		}


		if (is_last_offset)
			return get_prev_offset(state, id - deep + 1);


		char* label = malloc(strlen(node->token.value) + 1);
		strcpy(label, node->token.value);

		if (label[0] == '.' && last_label != NULL) {
			int l = strlen(label);
			label = realloc(label, strlen(last_label) + l + 1);
			memcpy(label + strlen(last_label), label, l);
			memcpy(label, last_label, strlen(last_label));
			label[l + strlen(last_label)] = 0;
		}

		for (int i = 0; i < state->semant_result.labels_count; i++) {
			if (strcmp(label, state->semant_result.labels[i].name) == 0) {
				free(label);
				return state->semant_result.labels[i].offset;
			}
		}

		add_error((Error){
		    .type = LABEL_NOT_FOUND,
		    .token = node->token
		});

		return 0;
	}

	if (node->token.type == PLUS) {
		long a = solve_expression(state, allow_labels, last_label, node->childs[1], id);
		long b = solve_expression(state, allow_labels, last_label, node->childs[0], id);
		return a + b;
	}

	if (node->token.type == MINUS) {
		long a = solve_expression(state, allow_labels, last_label, node->childs[1], id);
		long b = solve_expression(state, allow_labels, last_label, node->childs[0], id);
		return a - b;
	}

	if (node->token.type == STAR) {
		long a = solve_expression(state, allow_labels, last_label, node->childs[1], id);
		long b = solve_expression(state, allow_labels, last_label, node->childs[0], id);
		return a * b;
	}

	if (node->token.type == SLASH) {
		long a = solve_expression(state, allow_labels, last_label, node->childs[1], id);
		long b = solve_expression(state, allow_labels, last_label, node->childs[0], id);
		return a / b;
	}

	if (node->token.type == CARET) {
		long a = solve_expression(state, allow_labels, last_label, node->childs[1], id);
		long b = solve_expression(state, allow_labels, last_label, node->childs[0], id);
		return a ^ b;
	}

	if (node->token.type == PIPE) {
		long a = solve_expression(state, allow_labels, last_label, node->childs[1], id);
		long b = solve_expression(state, allow_labels, last_label, node->childs[0], id);
		return a | b;
	}

	if (node->token.type == AMPERSAND) {
		long a = solve_expression(state, allow_labels, last_label, node->childs[1], id);
		long b = solve_expression(state, allow_labels, last_label, node->childs[0], id);
		return a & b;
	}

	if (node->token.type == TILDA) {
		long a = solve_expression(state, allow_labels, last_label, node->childs[0], id);
		return ~a;
	}

	return 0;
}


char get_reg_id(char* s) {
	if (strcmp(s, "r0")  == 0) return 0;
	if (strcmp(s, "r1")  == 0) return 1;
	if (strcmp(s, "r2")  == 0) return 2;
	if (strcmp(s, "r3")  == 0) return 3;
	if (strcmp(s, "r4")  == 0) return 4;
	if (strcmp(s, "r5")  == 0) return 5;
	if (strcmp(s, "r6")  == 0) return 6;
	if (strcmp(s, "r7")  == 0) return 7;
	if (strcmp(s, "r8")  == 0) return 8;
	if (strcmp(s, "r9")  == 0) return 9;
	if (strcmp(s, "r10") == 0) return 10;
	if (strcmp(s, "r11") == 0) return 11;
	if (strcmp(s, "r12") == 0) return 12;
	if (strcmp(s, "r13") == 0) return 13;
	if (strcmp(s, "r14") == 0) return 14;
	if (strcmp(s, "r15") == 0) return 15;
	if (strcmp(s, "pc")  == 0) return 15;
	if (strcmp(s, "sp")  == 0) return 14;
	return 0;
}


int dec_len(long n) {
	return floor(log10(labs(n))) + 1;
}
