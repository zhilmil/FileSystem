#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include<assert.h>
#include<stdbool.h>
#include "libnetfiles.h"
#include <errno.h>
struct hostent *server;
int filemode;

int netserverinit(char* hostname, int* filemode)
{	
   server = gethostbyname(hostname);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      errno = HOST_NOT_FOUND;
      return -1;
   }
   else
	return 0;
}

int netopen(const char* pathname, int flags)
{
	int sockfd = connecting();
	int n;
	// creating to be sent message
	char* message;
	char* function = "open,";
	char* mode[4];
	snprintf(mode,4,"%d", flags); 
	message = malloc(strlen(pathname)+20);
	strcpy(message,function);
	strcat(message,mode);
	strcat(message,",");
	strcat(message,pathname);
	
	n = write(sockfd, message, strlen(message));
	
	if(n<0){
		perror("Error writing to socket");
		exit(1);
	}

	printf("going to read server response after read call\n");
	//reading server response
	char buffer[4];
	bzero(buffer,4);
	n = read(sockfd,buffer, 4);
	
	if(n<0)
	{
		perror("Error reading from socket");
		exit(1);
	}

	printf("%s\n",buffer);
	printf("open call is returning n client side");
	return atoi(buffer);	
}

ssize_t netread(int fildes, void* buf, size_t nbyte)
{
	int sockfd = connecting();
	int n;
	
	//creating to be sent message
	char* message;
	char* function = "read,";
	char s_nbyte[4];
	snprintf(s_nbyte,4,"%zu", nbyte);
	char s_fildes[4];
	snprintf(s_fildes,4,"%d", fildes); 
	
	//TODO define proper malloc
	message = malloc(100);
	strcpy(message, function);
	strcat(message, s_fildes);
	strcat(message, ",");
	strcat(message,s_nbyte);
	n = write(sockfd, message, strlen(message));
	if(n<0){
		perror("Error writing to socket");
		exit(1);
	}

	//reading server response
	char buffer[100];
	n = read(sockfd,buffer, 100);
	printf("server sent the message as follows after read \n");
	if(n<0)
	{
		perror("Error reading from socket");
		exit(1);
	}

   	printf("Buffer, %s\n",buffer);
	//parsing the message to get number of bytes
	char *token = strchr(buffer,',');
	char* path = token+1;
	*token = 0;
	int i=0;
	printf("%s bytes read by server\n",buffer);
	int bytes = atoi(buffer);
	if(bytes==-1)
	{
		int err = atoi(path);
		errno = err;
	}
	else
	strcpy(buf,path);
	return bytes;	

}

ssize_t netwrite(int fildes,const void* buf, size_t nbyte)
{
	int sockfd = connecting();
	int n;
	//creating to be sent message
	char* message;
	char* function = "write,";
	char s_fildes[4];
	snprintf(s_fildes,4,"%d", fildes); 
	char s_nbyte[4];
	snprintf(s_nbyte,4,"%zu", nbyte);
	
	//TODO define proper malloc
	message = malloc(100);
	strcpy(message, function);
	strcat(message, s_fildes);
	strcat(message, ",");
	strcat(message,s_nbyte);
	strcat(message,",");
	strcat(message,buf);
	
	n = write(sockfd, message, strlen(message));
	
	if(n<0){
		perror("Error writing to socket");
		exit(1);
	}
	//reading server response
	char buffer[4];
	n = read(sockfd,buffer, 4);
	
	if(n<0)
	{
		perror("Error reading from socket");
		exit(1);
	}

   
	//parsing the message to get number of bytes
	char *token = strchr(buffer,',');
	char* path = token+1;
	*token = 0;
	int bytes = atoi(buffer);
	if(bytes==-1)
	{
		int err = atoi(path);
		errno = err;
	}

	return bytes;
}


int netclose(int fd)
{
	int sockfd = connecting();
	int n;
	// creating to be sent message
	char* message;
	char* function = "close,";

	//TODO propeer malloc
	message = malloc(50);
	char s_fd[4];
	snprintf(s_fd,4,"%d", fd); 
	strcpy(message,function);
	strcat(message,s_fd);
	n = write(sockfd, message, strlen(message));
	
	if(n<0){
		perror("Error writing to socket");
		exit(1);
	}

	//reading server response
	char buffer[10];
	bzero(buffer,10);
	n = read(sockfd,buffer, 4);
	
	//parsing the message to get number of bytes
	char *token = strchr(buffer,',');
	char* path = token+1;
	*token = 0;

	int bytes = atoi(buffer);
	if(bytes<0)
	{
		int err = atoi(path);
		errno = err;
	}
	if(n<0)
	{
		perror("Error reading from socket");
		exit(1);
	}
	
	return bytes;	
}


int connecting()
{
	//creating socket
	int sockfd, portno,n;
	struct sockaddr_in serv_addr;
	portno = 1800;
	
	//creating socket point
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if (sockfd<0)
	{
		perror("Error opening socket");
		exit(1);
	}
	
	if(server == NULL)
	{
		perror("Error opening");
	}
	//getting addreses
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	
	//connecting to server
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0)
	{
		perror("connection error");
		exit(1);
	}

	return sockfd;
}
