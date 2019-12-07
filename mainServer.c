/*
** talker.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MYPORT "22236"
#define DBPORT "23236"	// the port users will be connecting to
#define CALCPORT "24236"
#define MAXBUFLEN 100

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int talk(int argc, char* argv[], char msg[], char port[]){
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	socklen_t addr_len;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo("127.0.01", port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}

	if ((numbytes = sendto(sockfd, msg, strlen(msg), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	freeaddrinfo(servinfo);

	//printf("talker: sent %d bytes to %s\n", numbytes, msg);



	//close socket
//	close(sockfd);
	return 0;
}


int main(int argc, char *argv[])
{
//	talk(argc, argv);
	//for (int i=0; i<30000; i++);
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];

	char capacity[10];
	char linkLength[10];
	char propVel[10];
	char propDelay[10];
	char transDelay[10];
	char totDelay[10];
	char inputID[100];
	char inputFileSize[100];
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);
	printf("The Server is up and running.\n");
	printf("Please input link ID:\n");
	scanf("%s %s", inputID, inputFileSize);
	//send data to dbServer
	printf("Link %s, file size %sMB.\n", inputID, inputFileSize);
	printf("Send Link %s to database server.\n", inputID);
	talk(argc, argv, inputID, DBPORT);

	int storeCount=0;
	int receiveCount=0;
	int dataToSend=0;
	int stall =0;


//active waiting
	while(1){
		// printf("listener: waiting to recvfrom...\n");

		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}

	//	printf("listener: got packet from %s\n",
			inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s);
  	// printf("listener: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';


		// printf("listener: packet contains \"%s\"\n", buf);
		if (strcmp(buf, "c\n")==0){
			printf("No match found\n");
			stall =0;
		}
		if(storeCount >0){
			//printf("Store count %d\n", storeCount);
			switch(storeCount){
				case 3:
					strcpy(capacity, buf);
					//printf("CAPACITY %s\n",capacity);
					storeCount--;
					break;
				case 2:
					strcpy(linkLength, buf);
				//	printf("LINKLENGTH %s\n", linkLength);
					storeCount--;
					break;
				case 1:
					strcpy(propVel, buf);
					printf("Receive link capacity %sMpbs, link length %skm, and propagation velocity %skm/s.\n", capacity, linkLength,propVel);
					//printf("PROPAGATIONVELOCITY %s\n", propVel);
					storeCount--;
					dataToSend=1;
					break;
				default:
					break;
			}
		}
		if(receiveCount >0){
			//printf("Receive count %d\n", storeCount);
			switch(receiveCount){
				case 3:
					strcpy(propDelay, buf);
					//printf("PROPAGATION DELAY %s\n",propDelay);
					receiveCount--;
					break;
				case 2:
					strcpy(transDelay, buf);
				//	printf("TRANSMISSION DELAY %s\n", transDelay);
					receiveCount--;
					break;
				case 1:
					strcpy(totDelay, buf);
					//printf("TOTAL DELAY %s\n", totDelay);
					printf("Receive transmission delay %sms, propagation delay %sms, total delay %sms.\n", transDelay, propDelay, totDelay);
					receiveCount--;
					stall=0;
					break;
				default:
					break;
			}
		}




	if (strcmp(buf, "a\0")==0){
		stall =1;
		//printf("I need to do something\n");
		storeCount=3;
	}

	if (strcmp(buf, "b\0")==0){
		//printf("I need to do something\n");
		receiveCount=3;
	}
	if (dataToSend){
		//printf("CAPACITY %s LINKLENGTH %s PROPVEL %s", capacity, linkLength, propVel);
		printf("Send information to calculation server.\n");
		talk(argc, argv, inputFileSize, CALCPORT);
		talk(argc, argv, capacity, CALCPORT);
		talk(argc, argv, linkLength, CALCPORT);
		talk(argc, argv, propVel, CALCPORT);
		dataToSend=0;
	}


	//	printf("talker: sent %d bytes to %s\n", numbytes, buf);


	if(stall==0){
		printf("Please input link ID:\n");
		scanf("%s %s", inputID, inputFileSize);
		//send data to dbServer
		printf("Link %s, file size %sMB.\n", inputID, inputFileSize);
		printf("Send Link %s to database server.\n", inputID);
		talk(argc, argv, inputID, DBPORT);
	}
}

	close(sockfd);
	return 0;

}
