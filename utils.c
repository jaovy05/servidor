#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include "utils.h"

void die(char *s)
{
    perror(s);
    exit(1);
}

int makeSocket(struct sockaddr_in si_me, int port){
    int s;
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) die("socket");

    // zera a var
    memset((char *) &si_me, 0, sizeof(si_me));
 
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
 
    //bind socket to port
    if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)  die("bind");
    return s;
}