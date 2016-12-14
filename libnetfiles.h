#ifndef LIBNETFILES
#define LIBNETFILES

#include<stdio.h>
#include<unistd.h>

//const char* server;
#define O_WRONLY 2
#define O_RDONLY 1
#define O_RDWR 4

int netserverinit(char* hostname, int* filemode);
int netopen(const char *pathname, int flags);
ssize_t netread(int fildes, void* buf, size_t nbyte); 
ssize_t netwrite(int fildes, const void* buf, size_t nbyte);
int netclose(int fd);


#endif
