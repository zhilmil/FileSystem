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

bool check_file_exists(char* filename)
{
	int i = 0;
	for(i=0;i<10;i++)
	{
		if(cach[i].filename == filename)
		{
			return true;
		}
	}
	return false;
}

md_file send_cach_file(char* filename)
{
	int i = 0;
	for(i=0;i<10;i++)
	{
		if(cach[i].filename == filename)
		{
			return cach[i];
		}
	}
}

void add_File(md_file file)
{
	//TODO handle multiple entries to cach
	cach[0] = file;
	printf("one  cahc entry edited with client%d and fd %d\n",cach[0].fd_client,cach[0].fileHandler);
}

int check_permission(int fd_client) {

	printf("checking permission for %d",fd_client);
	int i = 0;
	printf("one cach entry checking with client%d and fd %d\n",cach[0].fd_client,cach[0].fileHandler);
	for(i=0;i<10;i++)
	{
		if(cach[i].fd_client!=0 && cach[i].fd_client == fd_client)
		{
			printf("match obtained for cach\n %d,",cach[i].fileHandler );

			return cach[i].fileHandler;
		}
	}
	return -1;
}
