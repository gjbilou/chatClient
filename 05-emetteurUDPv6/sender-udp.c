#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
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

#define IP "::1"

int main(int argc, char *argv[])
{
	int portNbrDest = 1;

	/* test arg number */

	if (argc != 3)
	{
		fprintf(stderr, "usage: ./sender-udp ip_addr port_number\n");
		exit(EXIT_FAILURE);
	}

	/* convert and check port number */

	portNbrDest = atoi(argv[2]);
	if (!(portNbrDest >= 10000 && portNbrDest <= 65000))
	{
		fprintf(stderr, "Argument given is not a port number\n");
		exit(EXIT_FAILURE);
	}

	/* create socket */

	int sktCreated = socket(AF_INET6, SOCK_DGRAM, 0);
	CHECK(sktCreated);

	/* fill in dest IP and PORT */

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
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

	/* send message to remote peer */

	char msgSend[] = "hello world";
	int sizeMsgSend = strlen(msgSend);

	CHECK(sendto(sktCreated, msgSend, sizeMsgSend, 0, res->ai_addr, res->ai_addrlen));

	/* close socket */

	CHECK(close(sktCreated));

	/* free memory */
	freeaddrinfo(res);

	return 0;
}
