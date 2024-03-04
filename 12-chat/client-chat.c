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

#ifdef BIN

#define HELO 0xA
#define QUIT 0xB

#elif FILE
#define SEND_FILE_REQUEST 0x06
#define RECV_FILE_READY 0x07

void fileTransfer(char *file, char action, int skt, struct sockaddr *lAddr, socklen_t localAddrLen)
{

	FILE *fileToSend;
	char buffer[BUFF_SIZE] = {0};
	while (!(strlen(buffSender) == 1 && buffSender[0] == RECV_FILE_READY))
	{
		CHECK(recvfrom(skt, buffer, BUFF_SIZE, 0, NULL, NULL));
	}
	memset(buffer, 0, BUFF_SIZE);
	fileToSend = fopen(file, "r");
	if (fileToSend == NULL) 
	{
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	while (fgets(buffer, BUFF_SIZE, file) != NULL) {
		
	}

	fclose(fileToSend);
}

#else

#define PORT(p) htons(p)
#define HELO "/HELO"
#define QUIT "/QUIT"
#define BUFF_SIZE 1024

#endif

int main(int argc, char *argv[])
{
	int portNbr;
	/* test arg number */
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s port_number\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* convert and check port number */

	portNbr = atoi(argv[1]);
	if (!(portNbr >= 10000 && portNbr <= 65000))
	{
		fprintf(stderr, "port number invalid\n");
		exit(EXIT_FAILURE);
	}

	/* create socket */
	int skt;
	CHECK(skt = socket(AF_INET6, SOCK_DGRAM, 0));

	/* set dual stack socket */

	int value = 0;
	CHECK(setsockopt(skt, IPPROTO_IPV6, IPV6_V6ONLY, &value, sizeof value));

	/* set local addr */

	struct sockaddr_storage localAddr;
	socklen_t localAddrLen = sizeof(localAddr);
	memset(&localAddr, 0, localAddrLen);

	struct sockaddr *lAddr = (struct sockaddr *)&localAddr;
	struct sockaddr_in6 *lAddr6 = (struct sockaddr_in6 *)&localAddr;
	lAddr6->sin6_family = AF_INET6;
	lAddr6->sin6_port = PORT(portNbr);
	lAddr6->sin6_addr = in6addr_any;

	/* check if a client is already present */

	char buffer[BUFF_SIZE] = {0};
	if (bind(skt, lAddr, localAddrLen) == -1)
	{
		if (errno == EADDRINUSE)
		{
			CHECK(sendto(skt, HELO, strlen(HELO), 0, lAddr, localAddrLen));
		}
		else
		{
			perror("bind error");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		do
		{
			CHECK(recvfrom(skt, buffer, BUFF_SIZE, 0, lAddr, &localAddrLen));
		} while (strcmp(buffer, HELO));

		char remoteIP[NI_MAXHOST];
		char remotePort[NI_MAXSERV];
		int gniValue = getnameinfo(lAddr, localAddrLen, remoteIP, NI_MAXHOST, remotePort, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
		if (gniValue != 0)
		{
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(gniValue));
			exit(EXIT_FAILURE);
		}
		printf("%s %s\n", remoteIP, remotePort);
	}

	/* prepare struct pollfd with stdin and socket for incoming data */

	struct pollfd fds[2];

	// from stdin to send
	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;

	// from the socket to receive
	fds[1].fd = skt;
	fds[1].events = POLLIN;

	/* main loop */

	while (1)
	{
		char buffSender[BUFF_SIZE] = {0};
		char buffReceiver[BUFF_SIZE] = {0};

		CHECK(poll(fds, 2, -1));

		if (fds[0].revents && POLLIN)
		{
			CHECK(read(STDIN_FILENO, buffSender, BUFF_SIZE));
			CHECK(sendto(skt, buffSender, strnlen(buffSender, BUFF_SIZE), 0, lAddr, localAddrLen));

			// check if the message is QUIT
			if (strncmp(buffSender, QUIT, strlen(QUIT)))
			{
				break;
			}
#ifdef FILE
			else if (strlen(buffSender) == 1 && buffSender[0] == SEND_FILE_REQUEST)
			{
				char fileToSend[BUFF_SIZE] = {0};
				printf("path of file to send : ");
				// CHECK(read(STDIN_FILENO, fileToSend, BUFF_SIZE));
				if (!fgets(fileToSend, BUFF_SIZE, stdin))
				{
					fprintf(stderr, "filePath input failed\n");
					exit(EXIT_FAILURE);
				}
				fileTransfer(fileToSend, SEND_FILE_REQUEST, skt, lAddr, localAddrLen);
			}
#endif
		}
		else
		{
			CHECK(recvfrom(skt, buffReceiver, BUFF_SIZE, 0, NULL, NULL));
			if (strncmp(buffReceiver, QUIT, strlen(QUIT)) == 0)
			{
				break;
			}
			else
			{
				printf("%s", buffReceiver);
			}
		}
	}

	/* close socket */

	CHECK(close(skt));

	/* free memory */

	return 0;
}
