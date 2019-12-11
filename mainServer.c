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
#include <signal.h>
#include <sys/wait.h>

#define MYTCPPORT "21236"
#define MYPORT "22236"
#define DBPORT "23236"	// the port users will be connecting to
#define CALCPORT "24236"
#define MAXBUFLEN 100
#define BACKLOG 10

void sigchld_handler(int s)
{
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while(waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}
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
	int sockfd, new_fd, udp_fd;
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
	char client[10];
	char name[10];
	char toClient[500];
	// strcpy(name, argv[1]);
	int yes=1;
	struct sigaction sa;
	socklen_t sin_size;

	memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  if ((rv = getaddrinfo(NULL, MYTCPPORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }
  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
				perror("server: socket");
				continue;
			}
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
					perror("setsockopt");
					exit(1);
				}
				if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
					close(sockfd);
					perror("server: bind");
					continue;
				}
				break;
  }
	freeaddrinfo(servinfo); // all done with this structure
	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
	printf("The server is up and running.\n");


	while(1) { // main accept() loop
		char toClient[50];
		strcpy (toClient, "");
		int sendUDP= 0;
		int matchFound=0;

		//printf("Waiting...\n");
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

		if ((numbytes = recv (new_fd, buf, 10, 0))== -1){
			perror ("recv");
			exit(1);
		}
		else{
			// printf("Received inputID\n");
			buf[numbytes] = '\0';
			strcpy(inputID, buf);
			// printf("Input ID: %s\n", inputID);
		}

		if ((numbytes = recv (new_fd, buf, 10, 0))== -1){
			perror ("recv");
			exit(1);
		}
		else{
			// printf("Received input file size\n");
			buf[numbytes] ='\0';
			strcpy(inputFileSize, buf);
			// printf("Input File Size: %s\n", inputFileSize);
		}

		printf("Receive link %s, file size %sMB.\n", inputID, inputFileSize);
		// printf("Server: received '%s'\n",buf);

		//printf("Received greeting from '%s'\n",client);

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
			if ((udp_fd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
				perror("listener: socket");
				continue;
			}

			if (bind(udp_fd, p->ai_addr, p->ai_addrlen) == -1) {
				close(udp_fd);
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
		//printf("The Server is up and running.\n");
		// printf("Please input link ID:\n");
		// scanf("%s %s", inputID, inputFileSize);
		// //send data to dbServer
		//printf("Link %s, file size %sMB.\n", inputID, inputFileSize);
		printf("Send Link %s to database server.\n", inputID);
		talk(argc, argv, inputID, DBPORT);
		sendUDP=1;
		int storeCount=0;
		int receiveCount=0;
		int dataToSend=0;
		int stall =0;

		//sending UDP
	//active waiting
		while(sendUDP){
			// printf("listener: waiting to recvfrom...\n");

			addr_len = sizeof their_addr;
			if ((numbytes = recvfrom(udp_fd, buf, MAXBUFLEN-1 , 0,
				(struct sockaddr *)&their_addr, &addr_len)) == -1) {
				perror("recvfrom");
				exit(1);
			}

		//	printf("listener: got packet from %s\n",
				// inet_ntop(their_addr.ss_family,
				// 	get_in_addr((struct sockaddr *)&their_addr),
				// 	s, sizeof s);
			// printf("listener: packet is %d bytes long\n", numbytes);
			buf[numbytes] = '\0';


			// printf("listener: packet contains \"%s\"\n", buf);
			if (strcmp(buf, "c\n")==0){
				printf("No match found\n");
				stall =0;
				sendUDP=0;
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
						matchFound=1;
						receiveCount--;
						stall=0;
						sendUDP=0;
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
			// strcat(toClient, "Receive transmission delay ");
			// strcat(toClient, transDelay);
			// strcat(toClient, "ms, propagation delay ");
			// strcat(toClient, propDelay);
			// strcat(toClient, "ms and total delay ");
			// strcat(toClient, totDelay);
			// strcat(toClient, "ms\n");
			// printf("%s\n", toClient);
		}
	}
	close(udp_fd);

		//return information
		//printf("Name: %s\n", name);
		if (matchFound){
			// printf("Sending found information to client\n");
			if (send(new_fd, "d", 10 , 0) == -1)
			perror("send");
			for (int i=0;i < 30000; i++);
			if (send(new_fd, transDelay, 10 , 0) == -1)
			perror("send");
			for (int i=0;i < 30000; i++);
			if (send(new_fd, propDelay, 10 , 0) == -1)
			perror("send");
			for (int i=0;i < 30000; i++);
			if (send(new_fd, totDelay, 10 , 0) == -1)
			perror("send");

		}
		else{
			// printf("Sending no match found\n");
			if (send(new_fd, "No match found.\n", 10 , 0) == -1)
			perror("send");
			// printf("Send greetings to '%s'\n", client);
		}

		// if ((numbytes = recvfrom(new_fd, buf, 10 , 0,
		// 	(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		// 	perror("recvfrom");
		// 	exit(1);
		// }
		// buf[numbytes] = '\0';
		// printf("%s\n",buf);

	}

	close(sockfd);
	close(udp_fd);
	return 0;

}
