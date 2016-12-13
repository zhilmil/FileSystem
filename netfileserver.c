#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include<assert.h>
#include<stdbool.h>
#include "cache.h"
#include<fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<errno.h>

void *process_input(void *newsockfd);
void netopenserver(void *newsockfd, char* path);
void netcloseserver(void* newsockfd, char* msg);

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   //file descriptor
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 1800;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   
   //client listening
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while(1)
   {
	/* Accept actual connection from the client */
  	 newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
 	 if (newsockfd < 0) {
      		 perror("ERROR on accept");
      		 exit(1);
   	 }
	 pthread_t tempthread;
   	 int pc = pthread_create(&tempthread, NULL, process_input,(void*) newsockfd);
	 if(pc){
		perror("Error on creating pthread");
	}
	 	
   }   
   return 0;
}


void *process_input(void *newsockfd){

	int n;
	char buffer[256];
   	bzero(buffer,256);
   	n = read( newsockfd,buffer,255 );
   
   	if (n < 0) {
      		perror("ERROR reading from socket");
      		exit(1);
   	}
   
	
   	printf("Here is the message on server: %s Len:%d\n",buffer, strlen(buffer));
   
	//parsing the message
	char *token = strchr(buffer,',');
	char* path = token+1;
	*token = 0;
	char* leftovermessage = path;

	printf("parsed message buffer received function is %s\n",buffer);
	//calling appropriate function
	if(strcmp(buffer,"open")==0)
	{
		netopenserver(newsockfd,path);
		printf("called open fucntion\n");
	}
	
	if(strcmp(buffer,"read")==0)
	{
		netreadserver(newsockfd,path);
		printf("called read function\n");
	}
	
	if(strcmp(buffer,"close")==0)
	{
		netcloseserver(newsockfd,path);
		printf("called close function");
	}
	if(strcmp(buffer,"write")==0)
	{
		netwriteserver(newsockfd,path);
		printf("called write function");
	}
	
}

void netopenserver(void* newsockfd, char* path)
{
	
	printf("inside open on server and filename %s\n",path);

	//parsing the filename from mode + path
	char *token = strchr(path,',');
	char* file_name = token+1;
	*token = 0;
	printf("for read got filename %s\n",file_name);
	bool isOpened = check_open(file_name);
	int n;
	char buf[4];
	char* sentmessage = malloc(50);
	//file is open already
	if(isOpened==1)
   		n = write(newsockfd,"-1",4);
   	else
	{
		md_file file;
		if(check_file_exists(file_name))
		{
			file = send_cach_file(file_name); 
		}	
		int fileHandler = open(file_name,O_RDWR);	
		
		if(fileHandler<0)
		{
			printf("returned -1 fileHandler for initial open\n");
			printf("error%s",strerror(errno));
			strcpy(sentmessage,"-1");
			strcat(sentmessage,",");
			snprintf(buf,4,"%d",errno);
			strcat(sentmessage,buf);
		}
		else{
			printf("the filehandler given for client is %d\n for file %s",fileHandler,file_name);
			file.fileHandler = fileHandler;
			//assigning client id based on filehandler`:wq!
			int fd = (int)newsockfd*(-1);
			file.fd_client = fd;
			file.open = true;
			snprintf(buf,4,"%d", fd);
			strcpy(sentmessage,buf);
			strcat(sentmessage,",");
			add_File(file);
		}
		n = write(newsockfd,sentmessage,5);
	}
		
   	if (n < 0) {
      	perror("ERROR writing to socket");
      	exit(1);
	}
}
void netreadserver(void* newsockfd, char* message)
{
	
	printf("inside read on server");
	//parsing the message
	char *token = strchr(message,',');
	char* leftovermessage = token+1;
	*token = 0;
	
	int fd = atoi(message);
	int fileHandler = check_permission(fd);
	
	ssize_t sizeofbuff = atoi(leftovermessage);
	char buffer[sizeofbuff];
	char* sentmessage = malloc(50);
	int n,nwritten=0;
	char s_nwritten[4];
	if(fileHandler<0){
		printf("filehandler returned negative to read\n");
		n = write(newsockfd,"-1",4);
	}
	else {
		printf("number of bytes to bread %d",sizeofbuff);
		printf("reading with filehandler %d",fileHandler);
		while(nwritten<sizeofbuff && nwritten!=-1)
		nwritten = read(fileHandler,&buffer, sizeofbuff) + nwritten;
		if(nwritten ==-1)
		{
			printf("error%s",strerror(errno));
			strcpy(sentmessage,"-1,");
			snprintf(s_nwritten,4,"%d",errno);
			strcat(sentmessage,s_nwritten);
		}
		else
		{
			printf("number of read bytes on server side%d",nwritten);
			snprintf(s_nwritten,4,"%d",nwritten);
			printf("number of read bytes on server in stringside%s",s_nwritten);
			strcpy(sentmessage,s_nwritten);
			strcat(sentmessage,",");
			strcat(sentmessage,buffer);	
		}
		printf("sent message after read call finished %s\n",sentmessage);
		n = write(newsockfd,sentmessage,strlen(sentmessage)*sizeof(char));
	}

   	if (n < 0) {
      	perror("ERROR writing to socket");
      	exit(1);
	}
}
void netwriteserver(void* newsockfd, char* message)
{
	
	printf("inside write on server");
	//parsing the message
	char *token = strchr(message,',');
	char* leftovermessage = token+1;
	*token = 0;
	
	int fd = atoi(message);
	int fileHandler = check_permission(fd);
	
	printf("leftover message%s\n",leftovermessage);
	char* token1 = strchr(leftovermessage,',');
	char* buffer = token1 +1;
	printf("before token could be 0\n");
	*token1 = 0;
	printf("now token is zero %d",*token1);
	ssize_t sizeofbuff = (ssize_t)atoi(leftovermessage);
	printf("now buffer is to be written\n",buffer);
	printf("size of buffer is size %d\n",sizeofbuff);
	char* sentmessage = malloc(40);
	int n,nwritten=0;
	char s_nwritten[4];
	if(fileHandler<0){
		printf("filehandler returned negative to read\n");
		n = write(newsockfd,"-1",4);
	}
	else {
		printf("number of bytes to be written %d",sizeofbuff);
		printf("reading with filehandler %d",fileHandler);
		nwritten = write(fileHandler,buffer,sizeofbuff);
		if(nwritten ==-1)
		{
			printf("error%s",strerror(errno));
			strcpy(sentmessage,"-1,");
			snprintf(s_nwritten,4,"%d",errno);
			strcat(sentmessage,s_nwritten);
		}
		else
		{	
			printf("number of written bytes on server side%d",nwritten);
			snprintf(s_nwritten,4,"%d",nwritten);
			printf("number of written bytes on server in stringside%s",s_nwritten);
			strcpy(sentmessage,s_nwritten);
			strcat(sentmessage,",");
		}	
		printf("sent message after write call finished %s\n",sentmessage);
		n = write(newsockfd,sentmessage,strlen(sentmessage)*sizeof(char));
	}

   	if (n < 0) {
      	perror("ERROR writing to socket");
      	exit(1);
	}
}
void netcloseserver(void* newsockfd, char* msg)
{
	int fd = atoi(msg);
	printf("inside close on server and fd %d\n",fd);
	int n =0;
	int fileHandler = check_permission(fd);
	int close_return = close(fileHandler);	
	close_cach(fileHandler);
	char* sentmessage = malloc(40);
	char s_nwritten[4];
	if(close_return<0)
	{
		printf("close failed on server\n");
		strcpy(sentmessage,"-1,");
		snprintf(s_nwritten,4,"%d",errno);
		strcat(sentmessage,s_nwritten);
	}
	n = write(newsockfd,"0,",4);
   	if (n < 0) {
      	perror("ERROR writing to socket");
      	exit(1);
	}
}

