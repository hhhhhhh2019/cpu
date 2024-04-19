unsigned int foo(unsigned int a, unsigned int b) {
	unsigned int voidc = 0;

	while (a != b) {
		if (a > b)
			a -= b;
		else
			b -= a;

		voidc++;
	}

	bar(a,b,voidc + 2 * 3);

	return voidc;
}
