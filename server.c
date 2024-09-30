#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>



#define BUFFER_SIZE 1024
#define MAX_CLIENT 2

struct Point2D
{
	int x,y;
};


int main(int argc, char **argv)
{
    int sock, msg,i, msg_meteorite;
    char buffer[BUFFER_SIZE];
    bool new_client=false;
    bool game_started=false;
    bool in_game[MAX_CLIENT];
    bool status_game=false;
    bool first_message=true;
    struct sockaddr_in addr, last_client, input, client[MAX_CLIENT];
    int up_client=0;
    int in_len=sizeof(input);
    socklen_t addr_len = sizeof(addr);
    struct Point2D point[2];
    struct timeval timeout_game; 

    fd_set readfds;

    srand(time(NULL));

    if ( ( sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) < 0 ) 
    {
        perror("socket");
        exit(1);
    }

    else 
    {      //verifica se la sock è ok
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
    else 
    {     //verifica se la bind è ok
        printf("ok bind\n");
    }
    
    
 
    if (getsockname(sock, (struct sockaddr *)&addr, &addr_len) == -1) 
    {
        perror("Non riesco a trovare le info della socket");
        exit(1);
    } 
    else 
    {
        printf("Server in ascolto su %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }
    
    
    timeout_game.tv_sec=30;
    timeout_game.tv_usec=0;



    for(int j=0; j<MAX_CLIENT; j++)
    {
        in_game[j]=false;
    }

    time_t start_time = time(NULL);


    while(1)
    {

        memset(buffer, 0, BUFFER_SIZE);
        
        FD_ZERO(&readfds); 
        FD_SET(sock, &readfds);
        int selectfd=select(sock+1, &readfds,NULL, NULL, &timeout_game);
        if (selectfd <0 && errno!=EINTR)
        {
            perror("Errore nella select");
            exit(1);
        }

        if(FD_ISSET(sock, &readfds))
        {

            memset(buffer, 0, BUFFER_SIZE);

            msg=recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&input,&in_len);
            

			if(msg<0)
            {

				perror("errore nella ricezione del messaggio"); 
				exit(-1);
               
            }

            buffer[msg]=0;


            if(first_message || input.sin_port != last_client.sin_port || input.sin_addr.s_addr != last_client.sin_addr.s_addr)
            {
                printf("Ricevuto pacchetto da %s:%d\ndata:%s\n\n",inet_ntoa(input.sin_addr),ntohs(input.sin_port),buffer);

                last_client = input; 
                first_message=false;
            }

            else if (strcmp(buffer,"exit")==0)
            {
                for (int j=0; j<up_client; j++)
                {
                    if(input.sin_port==client[j].sin_port && input.sin_addr.s_addr == client[j].sin_addr.s_addr)
                    {
                        in_game[j]=false;
                        up_client--;
                    }
                }
            }

        }

        if (up_client<=0)
        {
            client[up_client].sin_family=AF_INET; 
            client[up_client].sin_port=input.sin_port;
            client[up_client].sin_addr.s_addr=input.sin_addr.s_addr;
            printf("Nuovo client connesso! IP: %s, PORTA: %d\n",
            inet_ntoa(client[up_client].sin_addr), ntohs(client[up_client].sin_port));
            in_game[up_client]=true;
            up_client++;

        }
        
        else
        {

            for( i=0; i<up_client; i++)
            {
                 if(client[i].sin_port==input.sin_port && client[i].sin_addr.s_addr==input.sin_addr.s_addr)
                 {
                    new_client=false;
                    break;
                 }  


                 else
				 {
                    new_client=true;
                 }   
                 
            }
            
            if (new_client==true && up_client<=MAX_CLIENT)
            {
                client[i]=input;
                printf("Nuovo client connesso! IP: %s, PORTA: %d\n",
                inet_ntoa(client[i].sin_addr), ntohs(client[i].sin_port));
                new_client=false;
                i=0;
                in_game[up_client]=true;
                up_client++;
            }

            if(new_client==true && up_client>=MAX_CLIENT)
            {
            	int index_full;
                for(index_full=0; index_full<MAX_CLIENT; index_full++)
                {
                    if(in_game[index_full]==false)
                    {
                        client[index_full]=input;
                        printf("Nuovo client connesso! IP: %s, PORTA: %d\n",
                        inet_ntoa(client[index_full].sin_addr), ntohs(client[index_full].sin_port));
                        new_client=false;
                        i=0;
                        in_game[up_client]=true;
                        up_client++;
                        break;
                    }

                }
                
                    if(index_full==MAX_CLIENT)
                    {
                        int msg_full;
                        char too_full[]="Troppi client connessi!";
                        msg_full=sendto(sock, too_full, sizeof(too_full), 0, (struct sockaddr *) &input, sizeof(input));
                    }
            }

        }

        if (game_started || time(NULL) - start_time >= 10)
        {
            game_started = true;
        }

        for (int j=0; j<2; j++)
		{
             point[j].x=rand()%8;
             point[j].y=rand()%8;
             printf("Punto generato %d:%d \n",point[j].x, point[j].y);
		}



        for (int k=0; k<MAX_CLIENT; k++)
        {
            if(in_game[k])
            {
                 msg_meteorite = sendto(sock, point, sizeof(point), 0, (struct sockaddr *) &client[k], sizeof (client[k]));
                 if (msg_meteorite < 0) 
                 { 
                    printf("Errore nell'invio del pacchetto: %s", strerror(errno));
                    exit(1);
                }
            }
           
        }


        timeout_game.tv_sec = 2;
        timeout_game.tv_usec = 0;


    }  

    close(sock);
    return 0;



 
 
      

}
