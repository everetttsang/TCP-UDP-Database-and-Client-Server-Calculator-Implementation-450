/*
** client.c -- a stream socket client demo // modified to monitor
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
#define PORT "25236" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define RECEIVESIZE 100
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
      int end =1;
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
       printf("The monitor is up and running\n");
        freeaddrinfo(servinfo); // all done with this structure

      //printf("Client sent greetings to the server\n");
      while(end){
        if ((numbytes = recv(sockfd, buf, RECEIVESIZE, 0)) == -1) {
          perror("recv");
        }
        buf[numbytes] = '\0';
        printf("%s\n",buf);
        if (strcmp(buf, "f\0")==0)
          end=0;
      }

    }

    close(sockfd);
    return 0;
  }
