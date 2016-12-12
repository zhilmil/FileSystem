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
	
	printf("buffer value remains same after call %s\n",buffer);
	if(strcmp(buffer,"read")==0)
	{
		netreadserver(newsockfd,path);
		printf("called read function\n");
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
		int fileHandler = open(file_name,O_RDWR|O_CREAT);	
		
		if(fileHandler<0)
		{
			printf("returned -1 fileHandler for initial open\n");

			printf("error%s",strerror(errno));

		}
		printf("the filehandler given for client is %d\n for file %s",fileHandler,file_name);
		file.fileHandler = fileHandler;
		char buf[4];
		int fd = (int)(newsockfd)*(-1);
		file.fd_client = fd;
		file.open = true;
		snprintf(buf,4,"%d", fd);

		add_File(file);
		
		n = write(newsockfd,buf,4);
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
	
	int sizeofbuff = atoi(leftovermessage);
	char buffer[sizeofbuff];
	
	int n,nwritten;
	if(fileHandler<0){
		printf("filehandler returned negative to read\n");
		n = write(newsockfd,"-1",4);
	}
	else {
		printf("number of bytes to bread %d",sizeofbuff);
		printf("reading with filehandler %d",fileHandler);
		nwritten = read(fileHandler,&buffer, sizeofbuff);
		if(nwritten ==-1)
		{
			printf("error%s",strerror(errno));

		}
		char* sentmessage = malloc(40);
		printf("number of read bytes on server side%d",nwritten);
		char s_nwritten[4];
		snprintf(s_nwritten,4,"%d",nwritten);
		printf("number of read bytes on server in stringside%s",s_nwritten);
		strcpy(sentmessage,s_nwritten);
		strcat(sentmessage,",");
		strcat(sentmessage,buffer);	
		printf("sent message after read call finished %s\n",sentmessage);
		n = write(newsockfd,sentmessage,strlen(sentmessage)*sizeof(char));
	}

   	if (n < 0) {
      	perror("ERROR writing to socket");
      	exit(1);
	}

void netwriteserver(void* newsockfd, char* message)
{
	
	printf("inside read on server");
	//parsing the message
	char *token = strchr(message,',');
	char* leftovermessage = token+1;
	*token = 0;
	
	int fd = atoi(message);
	int fileHandler = check_permission(fd);
	
	int sizeofbuff = atoi(leftovermessage);
	char buffer[sizeofbuff];
	
	int n,nwritten;
	if(fileHandler<0){
		printf("filehandler returned negative to read\n");
		n = write(newsockfd,"-1",4);
	}
	else {
		printf("number of bytes to bread %d",sizeofbuff);
		printf("reading with filehandler %d",fileHandler);
		nwritten = read(fileHandler,&buffer, sizeofbuff);
		if(nwritten ==-1)
		{
			printf("error%s",strerror(errno));

		}
		char* sentmessage = malloc(40);
		printf("number of read bytes on server side%d",nwritten);
		char s_nwritten[4];
		snprintf(s_nwritten,4,"%d",nwritten);
		printf("number of read bytes on server in stringside%s",s_nwritten);
		strcpy(sentmessage,s_nwritten);
		strcat(sentmessage,",");
		strcat(sentmessage,buffer);	
		printf("sent message after read call finished %s\n",sentmessage);
		n = write(newsockfd,sentmessage,strlen(sentmessage)*sizeof(char));
	}

   	if (n < 0) {
      	perror("ERROR writing to socket");
      	exit(1);
	}
}
}
