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

#define SIZE 100

void cpy(int src, int dst)
{
	ssize_t readFromFile;
	ssize_t writtenToSkt;
	char buff[SIZE];
	do
	{
		readFromFile = read(src, buff, SIZE);
		CHECK(readFromFile);

		writtenToSkt = write(dst, buff, readFromFile);
		CHECK(writtenToSkt);

	} while (readFromFile != 0);
	return;
}

int main(int argc, char *argv[])
{
	/* test arg number */

	if (argc != 2)
	{
		fprintf(stderr, "usage: ./client-http server_name\n");
		exit(EXIT_FAILURE);
	}

	/* get the list of struct addrinfo */

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	struct addrinfo *res;

	int rtrngai = getaddrinfo(argv[1], "http", &hints, &res);

	if (rtrngai)
	{
		fprintf(stderr, "%s\n", gai_strerror(rtrngai));
		exit(EXIT_FAILURE);
	}

	/* create socket */

	int skt = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	CHECK(skt);

	/* connect to the server */

	CHECK(connect(skt, res->ai_addr, res->ai_addrlen));

	/* prepare GET cmd */

	char strPrepared[SIZE];
	sprintf(strPrepared, "GET / HTTP/1.1\r\nHost: %s\r\nConnection:close\r\n\r\n", argv[1]);

	/* send GET cmd and get the response */

	CHECK(send(skt, strPrepared, strlen(strPrepared) + 1, 0));
	cpy(skt, STDOUT_FILENO);

	/* close socket */

	CHECK(close(skt));

	/* free struct addrinfo list */

	freeaddrinfo(res);

	return 0;
}
