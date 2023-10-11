#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#define CHECK(op)   do { if ( (op) == -1) { perror (#op); exit (EXIT_FAILURE); } \
                    } while (0)

#define IP   "127.0.0.1"

int main (int argc, char *argv [])
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
   
    /* complete sockaddr struct */

    struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
    struct addrinfo* res;



	if (getaddrinfo(IP, argv[1], &hints, &res))
	{
		fprintf(stderr, "getaddrinfo broke\n");
		exit(EXIT_FAILURE);
	}
	
    /* send message to remote peer */

    char msgSend[] = "hello world";
    int sizeMsgSend = strlen(msgSend);

    CHECK(sendto(sktCreated, msgSend, sizeMsgSend, 0, res->ai_addr, res->ai_addrlen));

    /* close socket */

    freeaddrinfo(res);
    close(sktCreated);

    /* free memory */

    return 0;
}
