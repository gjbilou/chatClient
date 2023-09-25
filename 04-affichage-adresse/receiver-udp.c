#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define CHECK(op)   do { if ( (op) == -1) { perror (#op); exit (EXIT_FAILURE); } \
                    } while (0)

#define IP   "127.0.0.1"
#define SIZE 100

int main (int argc, char *argv [])
{
    int portNbrDest = 1;
    
    /* test arg number */
    
    if (argc != 2)
    {
        perror("No port number indicated");
        exit(EXIT_FAILURE);
    }

    /* convert and check port number */

    portNbrDest = atoi(argv[1]);
    if (!(portNbrDest >= 10000 && portNbrDest <= 65000))
    {
        perror("Argument given is not a port number");
        exit(EXIT_FAILURE);
    }

    /* create socket */

    int sktCreated = socket(AF_INET, SOCK_DGRAM, 0);
    CHECK(sktCreated);
   
    /* complete struct sockaddr */

    // completer l'addresse a laquelle on va bind le socket. celle de notre
    // serveur 
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_DGRAM};
    struct addrinfo* res;

    CHECK(getaddrinfo(IP, argv[1], &hints, &res));

    
    /* link socket to local IP and PORT */

    CHECK(bind(sktCreated, res->ai_addr, res->ai_addrlen));

    /* wait for incoming message */

    char msgReceived[SIZE];
    
    struct sockaddr_storage src;
    socklen_t addrlen = sizeof(src);

    CHECK(recvfrom(sktCreated, msgReceived, SIZE, 0, (struct sockaddr*) &src, &addrlen));

    /* print sender addr and port */

    char srcIP[SIZE];
    char srcPORT[SIZE];
    CHECK(getnameinfo((struct sockaddr*) &src, addrlen, srcIP, SIZE, srcPORT, SIZE, NI_NUMERICHOST));
    // ici on doit print les info qu'on va recevoir avec la fonction
    // getnameinfo qu'on va pouvoir considerer comme l'inverse de la fonction
    // getaddrinfo. c'est comme la exp et log. 

    /* print received message */

    printf("%s\n", msgReceived);
    // system("clear");
    printf("%s %s", srcIP, srcPORT);

    /* close socket */

    close(sktCreated);

    /* free memory */

    freeaddrinfo(res);

    return 0;
}
