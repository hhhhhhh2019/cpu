#include "lexer.h"
#include <utils.h>
#include <as.h>

#include <math.h>
#include <stdio.h>
#include <string.h>


long dec2long(char* str) {
	long res = 0;

	while (*str) {
		res = res * 10 + (*str - '0');
		str++;
	}

	return res;
}

long hex2long(char* str) {
	str+=2;

	long res = 0;

	while (*str) {
		if (*str >= 'a')
			res = res * 16 + (*str - 'a' + 10);
		else
			res = res * 16 + (*str - '0');
		str++;
	}

	return res;
}

long bin2long(char* str) {
	str+=2;

	long res = 0;

	while (*str) {
		res = res * 2 + (*str - '0');
	}

	return res;
}


long get_long_size(long num) {
	if (num == 0)
		return 1;
	
	if (num > 0)
		return (ceil(log10(num))+1)*sizeof(char);
	else
		return (ceil(log10(-num))+1)*sizeof(char) + 1;
}



long get_number(Parser_state* state, Node* node, Node* parent, long last_addr) {
	if (node->value.type == DEC_NUMBER)
		return dec2long(node->value.value);
	if (node->value.type == BIN_NUMBER)
		return bin2long(node->value.value);
	if (node->value.type == HEX_NUMBER)
		return hex2long(node->value.value);
	if (node->value.type == CHARACTER)
		return node->value.value[1];
	if (node->value.type == UNDEFINED) {
		for (int i = 0; i < state->seresult.labels_count; i++) {
			if (strcmp(node->value.value, state->seresult.labels[i].name) == 0)
				return state->seresult.labels[i].offset;
		}

		fprintf(stderr, "%s %lu:%lu Метка не найдена!\n",
		    node->value.filepath,
		    node->value.line,
		    node->value.col);
		state->ok = 0;
	}

	if (node->value.type == CURRENTADDR) {
		return parent->offset;
	}

	if (node->value.type == LASTADDR) {
		return last_addr;
	}

	if (node->value.type == PLUS)
		return get_number(state, node->childs[1], parent, last_addr) + get_number(state, node->childs[0], parent, last_addr);
	if (node->value.type == MINUS)
		return get_number(state, node->childs[1], parent, last_addr) - get_number(state, node->childs[0], parent, last_addr);
	if (node->value.type == MULTIPLE)
		return get_number(state, node->childs[1], parent, last_addr) * get_number(state, node->childs[0], parent, last_addr);
	if (node->value.type == DIVIDE)
		return get_number(state, node->childs[1], parent, last_addr) / get_number(state, node->childs[0], parent, last_addr);
	if (node->value.type == AMPERSAND)
		return get_number(state, node->childs[1], parent, last_addr) & get_number(state, node->childs[0], parent, last_addr);
	if (node->value.type == PIPE)
		return get_number(state, node->childs[1], parent, last_addr) | get_number(state, node->childs[0], parent, last_addr);
	if (node->value.type == CARET)
		return get_number(state, node->childs[1], parent, last_addr) ^ get_number(state, node->childs[0], parent, last_addr);
	if (node->value.type == TILDA)
		return ~get_number(state, node->childs[0], parent, last_addr);

	return 0;
}


char get_reg_number(char* s) {
	if (strcmp(s, "r0") == 0)
		return 0;
	if (strcmp(s, "r1") == 0)
		return 1;
	if (strcmp(s, "r2") == 0)
		return 2;
	if (strcmp(s, "r3") == 0)
		return 3;
	if (strcmp(s, "r4") == 0)
		return 4;
	if (strcmp(s, "r5") == 0)
		return 5;
	if (strcmp(s, "r6") == 0)
		return 6;
	if (strcmp(s, "r7") == 0)
		return 7;
	if (strcmp(s, "r8") == 0)
		return 8;
	if (strcmp(s, "r9") == 0)
		return 9;
	if (strcmp(s, "r10") == 0)
		return 10;
	if (strcmp(s, "r11") == 0)
		return 11;
	if (strcmp(s, "r12") == 0)
		return 12;
	if (strcmp(s, "r13") == 0)
		return 13;
	if (strcmp(s, "r14") == 0)
		return 14;
	if (strcmp(s, "r15") == 0)
		return 15;
	if (strcmp(s, "pc") == 0)
		return 15;
	if (strcmp(s, "sp") == 0)
		return 14;
	return -1;
}
