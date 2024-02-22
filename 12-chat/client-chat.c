#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <poll.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define CHECK(op)               \
	do                          \
	{                           \
		if ((op) == -1)         \
		{                       \
			perror(#op);        \
			exit(EXIT_FAILURE); \
		}                       \
	} while (0)

#define PORT(p) htons(p)

int main(int argc, char *argv[])
{
	/* test arg number */

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s port_number\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* convert and check port number */

	int portNbr = atoi(argv[1]);
	if (!(portNbr >= 10000 && portNbr <= 65000))
	{
		fprintf(stderr, "Port number is incorrect\n");
		exit(EXIT_FAILURE);
	}
	
	/* create socket */

	int sktCreated = socket(AF_INET6, SOCK_DGRAM, 0);
	CHECK(sktCreated);

	/* set dual stack socket */

	int value = 0;
	CHECK (setsockopt (sktCreated, IPPROTO_IPV6, IPV6_V6ONLY, &value, sizeof value));

	/* set local addr */

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	// hints.ai_flags = AI_NUMERICHOST|AI_NUMERICSERV;
	struct addrinfo *res;

	int rtrnGAI = getaddrinfo(NULL, argv[1], &hints, &res);

	if (rtrnGAI)
	{
		fprintf(stderr, "%s\n", gai_strerror(rtrnGAI));
		exit(EXIT_FAILURE);
	}

	/* check if a client is already present */

	int rtrnBind = bind(sktCreated, res->ai_addr, res->ai_addrlen);
	if (rtrnBind == -1 && errno != EADDRINUSE)
	{
		CHECK(rtrnBind);
	}
	else if (rtrnBind == -1 && errno == EADDRINUSE)
	{
		// il faut attendre la reception du message /HELO
	}
	else
	{
		// il faut envoyer le message /HELO
	}
	

	/* prepare struct pollfd with stdin and socket for incoming data */

	/* main loop */

	/* close socket */

	/* free memory */

	return 0;
}
