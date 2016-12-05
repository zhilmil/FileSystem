#ifndef CACHE
#define CACHE

#include<stdio.h>
#include<unistd.h>

typedef struct{
	bool open = false;
	bool status = false;
} md_file;


typedef enum
