unsigned int foo(unsigned int a, unsigned int b, char (*f)(int)) {
	unsigned int c = 0;

	while (a != b) {
		if (a > b)
			a -= b;
		else
			b -= a;

		c++;
	}

	bar(a,b,c);

	return c;
}
