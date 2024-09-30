#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

struct Point2D
{
    int x,y;
};

int main(int argc, char **argv)
{
    int sock, write,meteorite,move;
    char sendbuff[4096];
    char game_ended[4]="exit";
    char game_status[100];
    int game;
    struct sockaddr_in client,server;
    socklen_t server_len = sizeof(server);
    struct Point2D point[2], ship, rand_pos;
    struct timeval timeout_game; 

    srand(time(NULL));

    ship.x=rand()%8;
    ship.y=rand()%8;

    rand_pos.x=0;
    rand_pos.y=0;

    	
	if (argc != 2) 
	{
		fprintf(stderr,"usage: %s <IPaddress>\n",argv[0]);
		exit (1);
	}


    if ( ( sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) < 0 )
	{
		perror("socket");
		exit(1);
    }
	
    else 
	{
		printf("ok sock\n");
		strcpy(sendbuff, "Sto iniziando la partita");
    }

    client.sin_family=AF_INET;
    client.sin_port=htons(8080);
    client.sin_addr.s_addr=htonl(INADDR_ANY);


    timeout_game.tv_sec=60;
    timeout_game.tv_usec=0;


	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout_game, sizeof(timeout_game)) < 0) 
	{
		perror("Error in setsockopt  del timeout_game");
		close(sock);
		exit(EXIT_FAILURE);
    }


	write = sendto(sock, sendbuff, strlen(sendbuff), 0, (struct sockaddr *) &client, sizeof(client));
	
    if (write < 0) 
	{     
		printf("Errore in scrittura: %s", strerror(errno));
		exit(1);
	}
    else 
	{
        printf("Messaggio inviato a %s:%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
    }

    printf("Posizione navicella %d:%d \n", ship.x, ship.y);

    game=recvfrom(sock, game_status, sizeof(game_status), 0, (struct sockaddr *)&server, &server_len);
	
    if(strcmp(game_status,"Troppi client connessi!")==0)
    {
        printf("%s\n",game_status );
        close(sock);
        return 0;
    }




    while(1)
    {
        meteorite=recvfrom(sock, point, sizeof(point), 0, (struct sockaddr *)&server, &server_len);
        if (meteorite<0)
        {

           if (errno == EAGAIN || errno == EWOULDBLOCK) // Timeout scaduto
            {
                printf("Timeout scaduto, il server non invia nulla \n");
                break;
            }

        }




		for (int j=0; j<2; j++)
        {
            if (ship.x==point[j].x && ship.y==point[j].y)
            {
                move=0;
                printf("Un meteorite sta arrivando verso la navicella! \n");
                move=rand()%2;
                if(move==1)
                {

					rand_pos.x=rand()%8;
					rand_pos.y=rand()%8;
					ship.x=rand_pos.x;
					ship.y=rand_pos.y;
					printf("La navicella si sta spostando \n");
					for(int k=0; k<2; k++)
					{
						if (ship.x==point[k].x && ship.y==point[k].y)
						{
							printf("La navicella si è spostata in un punto dove è presente un meteorite! \n");
							write = sendto(sock, game_ended, strlen(game_ended), 0, (struct sockaddr *) &client, sizeof(client));
							close(sock);
							return 0;

						}
					}

                         printf("La navicella si è spostata in %d:%d \n ", ship.x, ship.y);
                }

                else
                {
                    printf("La navicella ha deciso di non spostarsi \n");
                    write = sendto(sock, game_ended, strlen(game_ended), 0, (struct sockaddr *) &client, sizeof(client));
                    close(sock);
					return 0;
				}

            }
        }
    }
}
