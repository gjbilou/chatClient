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
    int portNbrUsr = 1;
    
    /* test arg number */
    
    if (argc != 2)
    {
        perror("No port number indicated");
        exit(EXIT_FAILURE);
    }

    /* convert and check port number */

    portNbrUsr = atoi(argv[1]);
    if (!(portNbrUsr >= 10000 && portNbrUsr <= 65000))
    {
        perror("Argument given is not a port number");
        exit(EXIT_FAILURE);
    }

    /* create socket */

    int sktCreated = socket(AF_INET, SOCK_DGRAM, 0);
    CHECK(sktCreated);
    /* le domaine indique quel protocole reseau a utiliser 
    le type indique quel socket (nature du socket) a creer 
    le protocole est la pour indiquer au socket quelle protocole a utilier par defaut parce que le type du socket ne connais pas le protocole a utiliser a l'origine */
    
    /* complete sockaddr struct */

    struct sockaddr_in receiver = {
        .
    };



    /* send message to remote peer */

    /* close socket */

    return 0;
}
