#include <utils.h>

#include <string.h>
#include <stdlib.h>


unsigned long size2int(char* str) {
	char last_char = str[strlen(str)-1];

	if (last_char < '0' || last_char > '9') {
		str[strlen(str)-1] = 0;
	}

	unsigned long result = atol(str);

	if (last_char == 'K')
		result *= 1024;
	if (last_char == 'M')
		result *= 1024 * 1024;
	if (last_char == 'G')
		result *= 1024 * 1024 * 1024;

	return result;
}
