#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include "cache.h"

md_file cach[10];

bool check_open(char* filename)
{
	int i = 0;
	for(i=0;i<10;i++)
	{
		if(cach[i].filename !=0 && cach[i].filename == filename)
		{
			return cach[i].open;
		}
	}
}

void init_cache() {
	memset(cach, 0, sizeof(cach));
}
