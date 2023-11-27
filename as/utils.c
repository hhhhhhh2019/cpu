#include <utils.h>

#include <math.h>


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



