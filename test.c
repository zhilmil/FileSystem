#include<stdio.h>
#include "libnetfiles.h"
#include<string.h>
#include<stdlib.h>
int main() {
	printf("reached here");
	int mode = 1;
        int i=0;
	netserverinit("java.cs.rutgers.edu",&mode);
	printf("checked server");
	printf("opening file");
	int fd = netopen("testfile2.txt", 0);
	printf("file descriptor received,%d",fd);
	char* message = malloc(100 *sizeof(char));
	printf("till here it was safe open call over\n");
	ssize_t temp =  netread(fd,message,40);
	printf("%s",message);
			
//	printf("file descriptor received,%d",fd);
	return 0;
}

