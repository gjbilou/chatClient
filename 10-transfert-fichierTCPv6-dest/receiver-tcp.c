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
#define SIZE 1024
#define QUEUE_LENGTH 1

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

	/* test arg number */

	if (argc != 3)
	{
		fprintf(stderr, "usage: %s ip_addr port_number\n", argv[0]);
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

	int sktListen = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	CHECK(sktListen);

	/* SO_REUSEADDR option allows re-starting the program without delay */
	int iSetOption = 1;
	CHECK(setsockopt(sktListen, SOL_SOCKET, SO_REUSEADDR, &iSetOption,
					 sizeof iSetOption));

	/* complete struct sockaddr */

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_NUMERICHOST|AI_NUMERICSERV;
	// il faut ajouter les flags pour qu'il n'ait par chercher dans le serveur dhcp pour la completion d'addresse
	// comme ca il ne travail que avec les addresse ip et les ports numeric

	struct addrinfo *res;

	int rtrngai = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rtrngai)
	{
		fprintf(stderr, "%s\n", gai_strerror(rtrngai));
		exit(EXIT_FAILURE);
	}

	/* link socket to local IP and PORT */

	CHECK(bind(sktListen, res->ai_addr, res->ai_addrlen));

	/* set queue limit for incoming connections */

	CHECK(listen(sktListen, QUEUE_LENGTH));

	/* wait for incoming TCP connection */

	struct sockaddr_storage src;
	socklen_t addrlen = sizeof(src);
	memset(&src, 0, sizeof(addrlen));

	int sktAccept = accept(sktListen, (struct sockaddr *) &src, &addrlen);
	CHECK(sktAccept);

	/* open local file */

	int fdCopy = open("copy.tmp", O_CREAT|O_WRONLY|O_TRUNC, 0644);
	CHECK(fdCopy);

	/* get the transmitted file */

	cpy(sktAccept, fdCopy);

	/* close sockets */
	
	CHECK(close(sktAccept));
	CHECK(close(sktListen));

	/* close file */

	CHECK(close(fdCopy));

	/* free memory */

	freeaddrinfo(res);

	return 0;
}
