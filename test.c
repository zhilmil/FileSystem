#include<stdio.h>
#include "libnetfiles.h"

int main() {
	printf("reached here");
	netserverinit("java.cs.rutgers.edu","exclusive");
	printf("checked server");
	printf("opening file");
	int fd = netopen("testfilelengt", 0);
	printf("file descriptor received,%d",fd);
	return 0;
}

