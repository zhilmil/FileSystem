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
	int fd = netopen("testxx4.txt", 0);
	printf("file descriptor received,%d",fd);
	char* message = "this is what i want to write please work";
	printf("till here it was safe open call over\n");
	ssize_t temp =  netwrite(fd,message,strlen(message));
	//printf("%d",temp);
	//netclose(fd);
	char* message1 = malloc(100 *sizeof(char));
	netread(fd,message1,strlen(message));
	netclose(fd);
	//netclose(		
//	printf("file descriptor received,%d",fd);
//	a
	return 0;
}

