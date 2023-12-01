#include <bin.h>
#include <utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



long last_addr;


void bin_parse_instr(Bin_result* result, Parser_state* state, Node* node) {
	char* buffer = malloc(node->size);
	char buffer_offset = 1;

	buffer[0] = node->code;

	Node* instr = node;

	node = node->childs[0];
	unsigned int arg_id = 0;

	char reg_id = 0;

	while (1) {
		Node* arg = node->childs[node->childs_count-1];

		if (arg->value.type == REGISTER) {
			if (reg_id == 0)
				buffer[buffer_offset++] = (get_reg_number(arg->value.value) & 15) << 4;
			else
				buffer[buffer_offset-1] |= get_reg_number(arg->value.value) & 15;

			reg_id = (reg_id + 1) % 2;
		} else {
			reg_id = 0;

			long number = get_number(state, arg, instr, last_addr);
			char size = 8;

			if (buffer[0] == 0x25 || buffer[0] == 0x3a)
				size = 1;

			for (int i = 0; i < size; i++)
				buffer[buffer_offset++] = (number >> i * 8) & 0xff;
		}

		if (node->childs_count == 1)
			break;

		node = node->childs[0];
	}

	result->data = realloc(result->data, result->data_size + instr->size);
	memcpy(result->data + result->data_size, buffer, instr->size);
	result->data_size += instr->size;
	free(buffer);
}


void bin_parse_data(Bin_result* result, Parser_state* state, Node* node) {
	int count = 1;

	if (node->childs_count == 3)
		count = get_number(state, node->childs[node->childs_count-1]->childs[0], node, last_addr);

	int one_size = 0;

	if (node->childs[1]->value.type == DB)
		one_size = 1;
	else if (node->childs[1]->value.type == DS)
		one_size = 2;
	else if (node->childs[1]->value.type == DI)
		one_size = 4;
	else if (node->childs[1]->value.type == DL)
		one_size = 8;

	char* buffer = malloc(node->size);
	char buffer_offset = 0;

	Node* elem = node->childs[0];

	while (1) {
		long number = get_number(state, elem->childs[elem->childs_count-1], node, last_addr);

		for (int j = 0; j < one_size; j++) {
			for (int i = 0; i < count; i++)
				buffer[buffer_offset + (node->size / count) * i] = (number >> j * 8) & 0xff;
			buffer_offset++;
		}

		if (elem->childs_count == 1)
			break;

		elem = elem->childs[0];
	}


	result->data = realloc(result->data, result->data_size + node->size);
	memcpy(result->data + result->data_size, buffer, node->size);
	result->data_size += node->size;
	free(buffer);
}


void bin_parse_node(Bin_result* result, Parser_state* state, Node* node) {
	if (node->value.type == Instr)
		bin_parse_instr(result, state, node);
	else if (node->value.type == Data)
		bin_parse_data(result, state, node);
	last_addr = node->offset;
}


Bin_result bin_parse(Parser_state* state) {
	Bin_result result = {
		malloc(0),
		0
	};


	last_addr = 0;


	Node* node = state->sresult.root;

	while (1) {
		if (node->childs_count == 1) {
			if (node->childs[0]->value.type == S) {
				node = node->childs[0];
				continue;
			}

			bin_parse_node(&result, state, node->childs[0]);

			break;
		}

		if (node->childs_count == 0)
			break;


		bin_parse_node(&result, state, node->childs[1]);

		node = node->childs[0];
	}


	return result;
}
