#ifndef CACHE
#define CACHE

#include<stdio.h>
#include<unistd.h>
#include<stdbool.h>

typedef struct{
	bool open;
	bool status;
	char* filename;
	int fd_client;
	int fileHandler;
	bool used;
} md_file;

void init_cache();
bool check_open(char* filename);
bool check_file_exists(char* filename);
md_file send_cach_file(char* filename);
int check_permission(int fd_client);
void add_File(md_file file);
#endif
