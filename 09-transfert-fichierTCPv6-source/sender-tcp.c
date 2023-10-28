#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
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
	int portNbrDest;
	int fileToReadFrom;

	/* test arg number */

	if (argc != 4)
	{
		fprintf(stderr, "usage: %s ip_addr port_number filename\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* convert and check port number */

	portNbrDest = atoi(argv[2]);
	if (!(portNbrDest >= 10000 && portNbrDest <= 65000))
	{
		fprintf(stderr, "Port number is incorrect\n");
		exit(EXIT_FAILURE);
	}

	/* open file to send */

	fileToReadFrom = open(argv[3], O_RDONLY);
	CHECK(fileToReadFrom);

	/* create socket */

	int sktCreated = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	CHECK(sktCreated);

	/* complete struct sockaddr */

	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_NUMERICHOST|AI_NUMERICSERV;
	struct addrinfo *res;

	int rtrngai = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rtrngai)
	{
		fprintf(stderr, "%s\n", gai_strerror(rtrngai));
		exit(EXIT_FAILURE);
	}

	/* connect to the remote peer */

	CHECK(connect(sktCreated, res->ai_addr, res->ai_addrlen));

	/* send the file content */

	cpy(fileToReadFrom, sktCreated);

	/* close socket */

	CHECK(close(sktCreated));

	/* close file */

	CHECK(close(fileToReadFrom));

	/* free memory */

	freeaddrinfo(res);

	return 0;
}
