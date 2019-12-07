/*
** listener.c -- a datagram sockets "server" demo
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


#define MYPORT "24236"	// the port users will be connecting to
#define SERVERPORT "22236"
#define CONTROL_SEND_CALC "a"


#define MAXBUFLEN 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int listening() {

return 0;
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

int main(int argc,char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];

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
	char capacity[10];
	char linkLength[10];
	char propVel[10];
	char fileSize[10];
	char propDelay[100];
	char transDelay[100];
	char totDelay[100];
	int storing=0;
	int storeCount=0;
	int compute=0;
	double propagationDelay;
	double transmissionDelay;
	double totalDelay;


printf("The calculation server is up and running.\n");
	while(1){
		//printf("listener: waiting to recvfrom...\n");

		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		else{
			if(storing==0){
				storing=1;
				storeCount=4;
				printf("Receive request from Main Server.\n");
			}
		}

	//	printf("listener: got packet from %s\n",
			inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s);
	//	printf("listener: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';

	//	printf("listener: packet contains \"%s\"\n", buf);







		if(storeCount >0){
		//	printf("Store count %d\n", storeCount);
			switch(storeCount){
				case 4:
					strcpy(fileSize, buf);
					//printf("FILE SIZE %s\n", fileSize);
					storeCount--;
					break;
				case 3:
					strcpy(capacity, buf);
				//	printf("CAPACITY %s\n",capacity);
					storeCount--;
					break;
				case 2:
					strcpy(linkLength, buf);
				//	printf("LINKLENGTH %s\n", linkLength);
					storeCount--;
					break;
				case 1:
					strcpy(propVel, buf);
				//	printf("PROPAGATIONVELOCITY %s\n", propVel);
					storeCount--;
				//	dataToSend=1;
					storing=0;
					compute =1;
					break;
				default:
					break;
			}
		}


	if(compute){

		propagationDelay = (atof(linkLength))/(atof(propVel))*100000;
		propagationDelay = (float)((int)(propagationDelay +0.05))/100;
		transmissionDelay = (atof(fileSize)*8)/ atof(capacity)*100000;
		transmissionDelay= (float)((int)(transmissionDelay +0.05))/100;
		totalDelay= propagationDelay + transmissionDelay;


	//	printf("pDelay %.2f tDelay %.2f totDelay %.2f\n", propagationDelay, transmissionDelay, totalDelay);

		sprintf(propDelay, "%.2f", propagationDelay);
		sprintf(transDelay, "%.2f", transmissionDelay);
		sprintf(totDelay, "%.2f", totalDelay);

	  //printf("proDelay %s transDelay %s totDelay %s\n", propDelay, transDelay, totDelay);
		talk(argc, argv, "b", SERVERPORT);
		talk(argc,argv,propDelay, SERVERPORT);
		talk(argc,argv,transDelay, SERVERPORT);
		talk(argc,argv,totDelay, SERVERPORT);
		printf("Send transmission delay %sms, propagation delay %sms, total delay %sms.\n", transDelay,propDelay, totDelay);


	compute=0;
	}
		//propDelay = d/s; distance (m) / link speed (m/s)
		//transDelay = L/R; Length of packet (bits) / Rate (Mbps)


	}
	close(sockfd);
	return 0;
}
