#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define CHECK(op)   do { if ( (op) == -1) { perror (#op); exit (EXIT_FAILURE); } \
                    } while (0)

#define IP      0x100007f /* 127.0.0.1 */
#define PORT(p) htons(p)

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
   
    /* complete sockaddr struct */

    struct sockaddr_storage dest;
    struct sockaddr_in* destPointer = (struct sockaddr_in*) &dest;
    destPointer->sin_family = AF_INET;
    destPointer->sin_port = PORT(portNbrDest);
    destPointer->sin_addr.s_addr = IP;

    /* send message to remote peer */

    char msgSend[] = "hello world";
    int sizeMsgSend = strlen(msgSend);

    CHECK(sendto(sktCreated, msgSend, sizeMsgSend, 0, (struct sockaddr*) destPointer, sizeof(*destPointer)));

    /* close socket */

    close(sktCreated);

    return 0;
}
