int foo(int a, int b) {
	int c = 0;

	while (a != b) {
		if (a > b)
			a -= b;
		else 
			b -= a;

		c++;
	}

	return c;
}
