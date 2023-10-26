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

	if (argc != 2)
	{
		fprintf(stderr, "No port number indicated\n");
		exit(EXIT_FAILURE);
	}

	/* convert and check port number */

	portNbrDest = atoi(argv[1]);
	if (!(portNbrDest >= 10000 && portNbrDest <= 65000))
	{
		fprintf(stderr, "Argument given is not a port number\n");
		exit(EXIT_FAILURE);
	}

	/* create socket */

	int sktCreated = socket(AF_INET, SOCK_DGRAM, 0);
	CHECK(sktCreated);

	/* complete struct sockaddr */

	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_NUMERICHOST|AI_NUMERICSERV;
	struct addrinfo *res;

	if (getaddrinfo(IP, argv[1], &hints, &res))
	{
		fprintf(stderr, "getaddrinfo broke\n");
		exit(EXIT_FAILURE);
	}

	/* link socket to local IP and PORT */

	CHECK(bind(sktCreated, res->ai_addr, res->ai_addrlen));

	/* wait for incoming message */

	char msgReceived[SIZE];

	CHECK(recvfrom(sktCreated, msgReceived, SIZE, 0, NULL, NULL));

	/* close socket */

	CHECK(close(sktCreated));

	/* free memory */

	freeaddrinfo(res);

	/* print received message */

	printf("%s\n", msgReceived);

	return 0;
}
