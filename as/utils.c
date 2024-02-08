#include <utils.h>
#include <lexer.h>


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
		if ('0' <= *s && *s <= '9')
			result += *s - '0';
		else
			result += *s - 'a' + 10;
		result *= 16;
		s++;
	}

	return result;
}


long bin2l(char* s) {
	long result = 0;

	while (*s) {
		result += *s - '0';
		result *= 2;
		s++;
	}

	return result;
}


long solve_expression(Node* node) {
	if (node->token.type == DEC_NUMBER)
		return dec2l(node->token.value);
	if (node->token.type == HEX_NUMBER)
		return hex2l(node->token.value);
	if (node->token.type == BIN_NUMBER)
		return bin2l(node->token.value);
	if (node->token.type == CHAR)
		return node->token.value[0];

	if (node->token.type == PLUS) {
		long a = solve_expression(node->childs[0]);
		long b = solve_expression(node->childs[1]);
		return a + b;
	}

	if (node->token.type == MINUS) {
		long a = solve_expression(node->childs[0]);
		long b = solve_expression(node->childs[1]);
		return a - b;
	}

	if (node->token.type == STAR) {
		long a = solve_expression(node->childs[0]);
		long b = solve_expression(node->childs[1]);
		return a * b;
	}

	if (node->token.type == SLASH) {
		long a = solve_expression(node->childs[0]);
		long b = solve_expression(node->childs[1]);
		return a / b;
	}

	if (node->token.type == CARET) {
		long a = solve_expression(node->childs[0]);
		long b = solve_expression(node->childs[1]);
		return a ^ b;
	}

	if (node->token.type == PIPE) {
		long a = solve_expression(node->childs[0]);
		long b = solve_expression(node->childs[1]);
		return a | b;
	}

	if (node->token.type == AMPERSAND) {
		long a = solve_expression(node->childs[0]);
		long b = solve_expression(node->childs[1]);
		return a & b;
	}

	if (node->token.type == TILDA) {
		long a = solve_expression(node->childs[0]);
		return ~a;
	}

	return 0;
}
