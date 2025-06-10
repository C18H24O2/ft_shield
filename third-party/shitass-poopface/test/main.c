#include "test-obf.h"
#include <stdio.h>

int main(int argc, char **argv) {
	printf("Hello from %s\n", argv[0]);

	for (int i = 0; i < 100; i++) {
		if (is_prime(i)) {
			printf("%d is prime let's fucking go\n", i);
		}
	}

	printf("Fuckin' fizz-buzz lookin ass test\n");
	return 0;
}
