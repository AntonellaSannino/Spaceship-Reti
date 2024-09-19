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
#define MAX_CLIENT 10

struct TwoDpoint
{
	int x,y;
};


int main(int argc, char **argv)
{
    int sock, msg,i, msg_meteorite;
    char buffer[BUFFER_SIZE];
    bool new_client=false;
    bool game_started=false;
    struct sockaddr_in addr, input, client[MAX_CLIENT];
    int up_client=0;
    int in_len=sizeof(input);
    socklen_t addr_len = sizeof(addr);
    struct TwoDpoint point[2];
    struct timeval timev; 

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
    
    
    timev.tv_sec=20;
    timev.tv_usec=0;

     if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timev, sizeof(timev)) < 0) {
    perror("Error in setsockopt");
    close(sock);
    exit(EXIT_FAILURE);
    }

    while(1) 
    {

    

        msg=recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&input,&in_len);
        if(msg<0)
        {

           if (errno == EAGAIN || errno == EWOULDBLOCK) // Timeout scaduto
            {
                printf("Timeout scaduto. Inizio della partita con %d client connessi.\n", up_client);
                break;
            }
        	
            else {

                perror("errore nella ricezione del messaggio");	
                exit(-1);
            }
        	
        }
    
        buffer[msg]=0;
    
        printf("ricevuto pacchetto da %s:%d\ndata:%s\n\n",inet_ntoa(input.sin_addr),ntohs(input.sin_port),buffer);
    
        
        if (up_client<=0)
        {
        	client[up_client].sin_family=AF_INET;
        	client[up_client].sin_port=input.sin_port;
         	client[up_client].sin_addr.s_addr=input.sin_addr.s_addr;
            printf("Nuovo client connesso! IP: %s, PORTA: %d\n",
            inet_ntoa(client[up_client].sin_addr), ntohs(client[up_client].sin_port));
           	up_client++;
        }
        
        else
        {

        	for( i=0; i<up_client; i++)
        	{
            	 if(client[i].sin_port==input.sin_port && client[i].sin_addr.s_addr==input.sin_addr.s_addr)
            	 {
            	  	new_client=false;
            	 }  


                 else{
                    new_client=true;
                 }   
            	 
        	}
        	
        	if (new_client==true && i<=MAX_CLIENT)
        	{
        		client[i]=input;
        		printf("Nuovo client connesso! IP: %s, PORTA: %d\n",
                inet_ntoa(client[i].sin_addr), ntohs(client[i].sin_port));
        		new_client=false;
        		i=0;
                up_client++;
        	}


            


        }





      
      
     
        
        
    }
    


    while(1)
    {
        printf("La partita inizia adesso! \n");
            game_started=true;
            //genero random
            while(game_started!=false)
            {
                sleep(2);
               for (int j=0; j<2; j++)
               {
                  point[j].x=rand()%8;
                  point[j].y=rand()%8;

                  printf("Punto generato %d:%d \n",point[j].x, point[j].y);
               }


            for (int k=0; k<up_client; k++)
            {
                msg_meteorite = sendto(sock, point, sizeof(point), 0, (struct sockaddr *) &client[k], sizeof (client[k]));
                 if (msg_meteorite < 0) {     
                printf("Errore nell'invio del pacchetto: %s", strerror(errno));
                exit(1);
                }
            }

    }
}

        
     

        



}
