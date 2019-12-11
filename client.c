/*
** client.c -- a stream socket client demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT "21236" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(int argc, char *argv[])
{
  char name[10];
  // strcpy(name, argv[1]);
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  char inputID[100];
	char inputFileSize[100];



    while(1){
      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
      }
      // loop through all the results and connect to the first we can
      for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
          p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
          }
          if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
          }
          break;
        }
        if (p == NULL) {
          fprintf(stderr, "client: failed to connect\n");
          return 2;
        }
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
        s, sizeof s);
       printf("The client is up and running\n");
        freeaddrinfo(servinfo); // all done with this structure
      printf("Please input link ID:\n");
      scanf("%s %s", inputID, inputFileSize);
      //send data to dbServer
      printf("Send Link %s and file size %sMB to the main server.\n", inputID, inputFileSize);
      //printf("Send Link %s to database server.\n", inputID);

      if (send (sockfd, inputID,10, 0)==-1)
        perror("send");


      if (send (sockfd, inputFileSize,10, 0)==-1)
        perror("send");


      //printf("Client sent greetings to the server\n");
      if ((numbytes = recv(sockfd, buf, 10, 0)) == -1) {
        perror("recv");
      }
      buf[numbytes] = '\0';
    //  printf("Get reply from '%s'\n",buf);
      if (strcmp(buf, "d\0")==0){
        char tempTransDelay [10];
        char tempPropDelay[10];
        char tempTotDelay[10];

        if ((numbytes = recv(sockfd, buf, 10, 0)) == -1) {
          perror("recv");
        }
        buf[numbytes] = '\0';
        strcpy(tempTransDelay, buf);
        // printf("%s\n", tempTransDelay);

        if ((numbytes = recv(sockfd, buf, 10, 0)) == -1) {
          perror("recv");
        }
        buf[numbytes] = '\0';
        strcpy(tempPropDelay, buf);
        // printf("%s\n", tempPropDelay);

        if ((numbytes = recv(sockfd, buf, 10, 0)) == -1) {
          perror("recv");
        }
        buf[numbytes] = '\0';
        strcpy(tempTotDelay, buf);
        //printf("%s\n", tempTotDelay);
        printf("Receive transmission delay %sms, propagation delay %sms and total delay %sms\n", tempTransDelay,tempPropDelay,tempTotDelay);
      }
      else{
        printf("No match found.\n");
      }

    //   if (send (sockfd, "thanks\0",10, 0)==-1)
    //     perror("send");
    }

    close(sockfd);
    return 0;
  }
