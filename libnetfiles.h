#ifndef LIBNETFILES
#define LIBNETFILES

#include<stdio.h>
#include<unistd.h>

//const char* server;

int netserverinit(char* hostname, int* filemode);
int netopen(const char *pathname, int flags);


#endif
