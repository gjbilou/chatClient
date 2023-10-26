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

int main(int argc, char *argv[])
{
	int portNbrDest;

	/* test arg number */

	if (argc != 3)
	{
		fprintf(stderr, "usage: ./sender-tcp ip_addr port_number\n");
		exit(EXIT_FAILURE);
	}

	/* convert and check port number */

	portNbrDest = atoi(argv[2]);
	if (!(portNbrDest >= 10000 && portNbrDest <= 65000))
	{
		fprintf(stderr, "Port number is incorrect\n");
		exit(EXIT_FAILURE);
	}

	/* create socket */

	int sktCreated = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	CHECK(sktCreated);

	/* complete struct sockaddr */

	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	struct addrinfo *res;

	int rtrngai = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rtrngai)
	{
		fprintf(stderr, "%s\n", gai_strerror(rtrngai));
		exit(EXIT_FAILURE);
	}

	/* connect to the remote peer */

	CHECK(connect(sktCreated, res->ai_addr, res->ai_addrlen));

	/* send the message */

	char msgSend[] = "hello world";
	int sizeMsgSend = strlen(msgSend);

	CHECK(send(sktCreated, msgSend, sizeMsgSend, 0));

	/* close socket */

	CHECK(close(sktCreated));

	/* free memory */

	freeaddrinfo(res);

	return 0;
}
