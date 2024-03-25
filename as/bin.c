#include "lexer.h"
#include <bin.h>
#include <errno.h>
#include <utils.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static char* output;
static unsigned long output_size;
static char* last_label;


static void parse_data(Compiler_state* state, Node* node, unsigned long id) {
	unsigned long times = 1;
	Node* root = node;

	if (node->token.type == TIMES) {
		times = solve_expression(state, 1, last_label, node->childs[0], id);
		node = node->childs[1];
	}

	char* buffer = malloc(root->size);
	unsigned long buffer_offset = 0;

	while (1) {
		char size = 0;

		if (strcmp(node->token.value, "dl") == 0)
			size = 8;
		else if (strcmp(node->token.value, "di") == 0)
			size = 4;
		else if (strcmp(node->token.value, "ds") == 0)
			size = 2;
		else if (strcmp(node->token.value, "db") == 0)
			size = 1;

		for (int i = 0; i < node->childs[0]->childs_count; i++) {
			if (node->childs[0]->childs[i]->token.type == STRING) {
				for (int n = 0; n < strlen(node->childs[0]->childs[i]->token.value); n++) {
					long val = (long)node->childs[0]->childs[i]->token.value[n] & 0xff;

					if (val == '\\') {
						n++;

						if (node->childs[0]->childs[i]->token.value[n] == 'a')
							val = '\a';
						if (node->childs[0]->childs[i]->token.value[n] == 'b')
							val = '\b';
						if (node->childs[0]->childs[i]->token.value[n] == 'f')
							val = '\f';
						if (node->childs[0]->childs[i]->token.value[n] == 'n')
							val = '\n';
						if (node->childs[0]->childs[i]->token.value[n] == 'r')
							val = '\r';
						if (node->childs[0]->childs[i]->token.value[n] == 't')
							val = '\t';
						if (node->childs[0]->childs[i]->token.value[n] == 'v')
							val = '\v';
						if (node->childs[0]->childs[i]->token.value[n] == '\\')
							val = '\\';
						if (node->childs[0]->childs[i]->token.value[n] == '\'')
							val = '\'';
						if (node->childs[0]->childs[i]->token.value[n] == '"')
							val = '\"';
						if (node->childs[0]->childs[i]->token.value[n] == '?')
							val = '\?';
						if (node->childs[0]->childs[i]->token.value[n] == '0')
							val = '\0';
						if (node->childs[0]->childs[i]->token.value[n] == 'e')
							val = '\e';
					}

					for (int j = 0; j < size; j++) {
						for (int k = 0; k < times; k++)
							buffer[buffer_offset + root->size / times * k] =
							    (val >> j * 8) & 0xff;
						buffer_offset++;
					}
				}

				continue;
			}

			long val = solve_expression(state, 1, last_label, node->childs[0]->childs[i], id);


			for (int j = 0; j < size; j++) {
				for (int k = 0; k < times; k++)
					buffer[buffer_offset + root->size / times * k] = (val >> j * 8) & 0xff;
				buffer_offset++;
			}

			// printf("%ld\n", val);
			// memcpy(buffer + buffer_size - size, &val, size);
		}

		if (node->childs_count == 1)
			break;

		node = node->childs[1];
	}


	output_size += root->size;
	output = realloc(output, output_size);
	memcpy(output + output_size - root->size, buffer, root->size);

	free(buffer);
}


static void parse_instruction(Compiler_state* state, Node* node, unsigned long id) {
	char* buffer = malloc(node->size);
	char buffer_offset = 1;

	buffer[0] = node->code;

	char reg_id = 0;

	for (int i = 0; i < node->childs_count; i++) {
		Node* arg = node->childs[i];

		if (arg->token.type == REGISTER) {
			if (reg_id == 0)
				buffer[buffer_offset++] = (get_reg_id(arg->token.value) & 15) << 4;
			else
				buffer[buffer_offset-1] |= get_reg_id(arg->token.value) & 15;

			reg_id = (reg_id + 1) % 2;

			continue;
		}

		reg_id = 0;

		long val = solve_expression(state, 1, last_label, arg, id);

		char size = 8;

		if (buffer[0] == 0x25 || buffer[0] == 0x3a)
			size = 1;

		for (int j = 0; j < size; j++)
			buffer[buffer_offset++] = (val >> j * 8) & 0xff;
	}

	output_size += node->size;
	output = realloc(output, output_size);
	memcpy(output + output_size - node->size, buffer, node->size);

	free(buffer);
}


static void parse_node(Compiler_state* state, Node* node, unsigned long id) {
	if (node->type == SLABEL) {
		if (node->token.value[0] != '.')
			last_label = node->token.value;

		return;
	}

	if (node->type == SINSTRUCTION)
		parse_instruction(state, node, id);

	if (node->type == SDATA)
		parse_data(state, node, id);
}


void bin(Compiler_state* state) {
	output = malloc(0);
	output_size = 0;
	last_label = NULL;

	for (int i = 0; i < state->synt_result.nodes_count; i++)
		parse_node(state, state->synt_result.nodes[i], i);


	// for (int i = 0; i < output_size; i++) {
	// 	printf("%02hhx ", output[i]);
	// }

	FILE* f = fopen(state->output_filename, "wb");

	if (f == NULL) {
		ERROR("Cannot open file \"%s\": %s\n", state->output_filename, strerror(errno));
#ifdef DEBUG
		perror("fopen " STR(__FILE__) " " STR(__LINE__));
#endif
		exit(errno);
	}

	fwrite(output, 1, output_size, f);

	fclose(f);
}
