/*
    Simple udp server
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define BUFLEN 101  //Max length of buffer
#define PORT 20434   //The port on which to listen for incoming data
 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
void sender(){
    while (1){

    }
}

void receiver(){
    while (1){

    }
}


int main(void){
    struct sockaddr_in si_me, si_other;
    int sockect, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
    char message[BUFLEN];

    // para o prinft n se cagar inteiro
    setvbuf(stdout, 0, _IONBF, 0) ;


    // cria o sockect
    if ((sockect=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) die("socket");
    
     
    // zera a var
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if(bind(sockect, (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)  die("bind");
    
    close(sockect);

    pthread_t t_receiver, t_sender;

    pthread_create(&t_receiver, NULL, receiver, NULL);

    // Cria thread de envio
    pthread_create(&t_sender, NULL, sender, NULL);

    pthread_join(t_receiver, NULL); // para n encerrar a thread principal sem as outras encerrarem 
    pthread_join(t_sender, NULL);
    return 0;
}
