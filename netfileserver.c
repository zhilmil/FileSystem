#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include<assert.h>
#include<stdbool.h>
#include "cache.h"

void *process_input(void *newsockfd);

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
   
	
   	printf("Here is the message: %s Len:%d\n",buffer, strlen(buffer));
   
	//parsing the message
	char *token = strchr(buffer,',');
	char* path = token+1;
	*token = 0;
	
	printf("mode:%s, path:%s\n", buffer, path);
	
	bool open = check_open(path);

	//file is open already
	if(open==1)
   		n = write(newsockfd,"-1",4);
   	else
	{
		//send file descriptor
		//TODO update file status
		char buf[4];
		int fd = (int)(newsockfd)*(-1);
		snprintf(buf,4,"%d", fd);
		n = write(newsockfd,buf,4);
	}
		
   	if (n < 0) {
      	perror("ERROR writing to socket");
      	exit(1);
	}
}


