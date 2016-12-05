#ifndef CACHE
#define CACHE

#include<stdio.h>
#include<unistd.h>
#include<stdbool.h>

typedef struct{
	bool open;
	bool status;
	char* filename;
	int fd;
} md_file;

void init_cache();
bool check_open(char* filename);

bool add_file(struct md_file file);

#endif
