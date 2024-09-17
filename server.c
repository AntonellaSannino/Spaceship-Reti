#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv)
{
    int sock, msg;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in addr, input;
    int in_len=sizeof(input);
    

    if ( ( sock = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 ) {
    perror("socket");
    exit(1);
    }
    else {      //verifica se la sock è ok
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
    else {     //verifica se la bind è ok
    printf("ok bind\n");
    }
    
    while(1) {
    msg=recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&input,&in_len);
    if(msg<0){
    	perror("errore nella ricezione del messaggio");	
    	exit(-1);
    }
    buffer[msg]=0;
    printf("ricevuto pacchetto da %s:%d\ndata:%s\n\n",inet_ntoa(input.sin_addr),ntohs(input.sin_port),buffer);
    }
    
}
