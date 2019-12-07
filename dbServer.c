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


#define MYPORT "23236"	// the port users will be connecting to
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
	printf("The database server is up and running.\n");

	while(1){
		//printf("listener: waiting to recvfrom...\n");

		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		else{
			printf("Received request from Main Server.\n");
		}

	//	printf("listener: got packet from %s\n",
			inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s);
	//	printf("listener: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';

	//	printf("listener: packet contains \"%s\"\n", buf);
		char id[10];
		char* capacity;
		char* linkLength;
		char* propVel;
		strcpy(id, buf);
	//	printf("Copied: %s\n", id);


		//printf("talker: sent %d bytes to %s\n", numbytes, buf);
		//read file
		/*
		https://riptutorial.com/c/example/8274/get-lines-from-a-file-using-getline--
		*/
		char *line_buf = NULL;
		size_t line_buf_size=0;
		int line_count=0;
		ssize_t line_size;

		FILE *dbFile;
		dbFile = fopen("database.txt", "r");
		if(!dbFile){
			return 1;
		}
		line_size= getline(&line_buf, &line_buf_size, dbFile);
		/* Loop through until we are done with the file. */
	int foundID=0;
  while (line_size >= 0)
  {
    /* Increment our line count */
    line_count++;

    /* Show the line details */
    //printf("contents: %s", line_buf);
		char *token = strtok(line_buf, " ");
	//	printf("ID %s TOKEN %s\n", id, token);
		if (strcmp(id, token)==0){
			//printf("Obtained Line: %s\n", line_buf);
			capacity= strtok(NULL, " ");
			linkLength= strtok(NULL," ");
			propVel= strtok(NULL," ");
			//printf("CAPACITY %s LINKLENGTH %s PROPVEL %s", capacity, linkLength, propVel);
			talk(argc , argv, CONTROL_SEND_CALC, SERVERPORT);
			//talk(argc, argv, id, SERVERPORT);
			talk(argc, argv, capacity, SERVERPORT);
			talk(argc, argv, linkLength, SERVERPORT);
			talk(argc, argv, propVel, SERVERPORT);
			printf("Send link %s, capacity %sMbps, link length %skm, propagation velocity %skm/s.\n", id, capacity,linkLength,propVel);

			line_size=-1;
			foundID=1;
		}

    /* Get the next line */
    line_size = getline(&line_buf, &line_buf_size, dbFile);
  }
	/* Free the allocated line buffer */
	free(line_buf);
	line_buf = NULL;

	/* Close the file now that we are done with it */
	fclose(dbFile);

	if(foundID){

	}
	else{
		talk(argc, argv, "c\n",SERVERPORT);
		printf("No match found.\n");
	}




	}
	close(sockfd);
	return 0;
}
