#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>


struct Point2D
{
    int x,y;
};

int main(int argc, char **argv)
{
    int sock, write,meteorite;
    char sendbuff[4096];
    struct sockaddr_in client,server;
    struct Point2D point[2], ship;
    
    ship.x=0;
    ship.y=0;
    	
  if (argc != 2) {
    fprintf(stderr,"usage: %s <IPaddress>\n",argv[0]);
    exit (1);
  }

    if ( ( sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) < 0 ) {
    perror("socket");
    exit(1);
    }
    else {
    printf("ok sock\n");
    strcpy(sendbuff, "Sto iniziando la partita");
    }

    client.sin_family=AF_INET;
    client.sin_port=htons(8080);
    client.sin_addr.s_addr=htonl(INADDR_ANY);

	write = sendto(sock, sendbuff, strlen(sendbuff), 0, (struct sockaddr *) &client, sizeof(client));
    if (write < 0) {     
			printf("Errore in scrittura: %s", strerror(errno));
			exit(1);
	}
    else {
        printf("Messaggio inviato a %s:%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
    }

    printf("Continuo\n");

    while(1)
    {
        meteorite=recvfrom(sock, point, sizeof(point), 0, (struct sockaddr *)&server, sizeof(server));

      
             for (int j=0; j<2; j++)
        {
            printf("Nuovo punto ricevuto %d,%d \n", point[j].x, point[j].y);
        }

        
     

    }

    
}
