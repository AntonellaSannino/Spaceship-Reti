#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char **argv)
{
    int sock;
    
    struct sockaddr_in addr;

    if ( ( sock = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 ) {
    perror("socket");
    exit(1);
    }
    else {
    printf("ok sock\n");
    }

    addr.sin_family=AF_INET;
    addr.sin_port=htons(8080);
    addr.sin_addr.s_addr=htonl(INADDR_ANY);

    if(bind(sock, (struct sockaddr *) &addr, sizeof(addr))<0)
    {
    perror("bind");
    exit(1);
    }
    else {
    printf("ok bind\n");
    }
    
}
