#include<stdio.h>
#include "libnetfiles.h"

int main() {
	printf("reached here");
	netserverinit("java.cs.rutgers.edu","exclusive");
	printf("checked server");
	netopen("testfilelengt", 0);
	return 0;
}

