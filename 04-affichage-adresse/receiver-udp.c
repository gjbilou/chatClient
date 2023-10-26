#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define CHECK(op)               \
	do                          \
	{                           \
		if ((op) == -1)         \
		{                       \
			perror(#op);        \
			exit(EXIT_FAILURE); \
		}                       \
	} while (0)

#define IP "127.0.0.1"
#define SIZE 100

int main(int argc, char *argv[])
{
	int portNbrDest = 1;

	/* test arg number */

	if (argc != 3)
	{
		fprintf(stderr, "usage: ./receiver-udp ip_addr port_number\n");
		exit(EXIT_FAILURE);
	}

	/* convert and check port number */

	portNbrDest = atoi(argv[2]);
	if (!(portNbrDest >= 10000 && portNbrDest <= 65000))
	{
		fprintf(stderr, "port value erroneous\n");
		exit(EXIT_FAILURE);
	}

	/* create socket */

	int sktCreated = socket(AF_INET, SOCK_DGRAM, 0);
	CHECK(sktCreated);

	/* complete struct sockaddr */

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_NUMERICHOST|AI_NUMERICSERV;
	struct addrinfo *res;

	int rtrnGAI = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rtrnGAI)
	{
		fprintf(stderr, "%s\n", gai_strerror(rtrnGAI));
		exit(EXIT_FAILURE);
	}

	/* link socket to local IP and PORT */

	CHECK(bind(sktCreated, res->ai_addr, res->ai_addrlen));

	/* wait for incoming message */

	char *msgReceived = calloc(SIZE + 1, sizeof(*msgReceived));

	struct sockaddr_storage src;
	socklen_t addrlen = sizeof(src);
	memset(&src, 0, sizeof(addrlen));

	CHECK(recvfrom(sktCreated, msgReceived, SIZE, 0, (struct sockaddr *)&src, &addrlen));

	/* print sender addr and port */

	char srcIP[NI_MAXHOST];
	char srcPORT[NI_MAXSERV];

	int rtrnGNI = getnameinfo((struct sockaddr *)&src, addrlen, srcIP, NI_MAXHOST, srcPORT, NI_MAXSERV, NI_NUMERICHOST);

	if (rtrnGNI)
	{
		fprintf(stderr, "%s\n", gai_strerror(rtrnGNI));
		exit(EXIT_FAILURE);
	}

	printf("%s\n", msgReceived);
	printf("%s %s", srcIP, srcPORT);

	/* close socket */

	CHECK(close(sktCreated));

	/* free memory */

	free(msgReceived);
	freeaddrinfo(res);

	/* print received message */

	return 0;
}
