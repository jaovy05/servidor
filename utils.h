#include <stdio.h>
#include <arpa/inet.h>

#ifndef T_UINT

#define T_UINT
typedef unsigned int uint;

#endif // T_UINT

#ifndef F_UTILS

#define F_UTILS

void die(char *s);
int makeSocket(struct sockaddr_in si_me, int port);
void readRoteadores(uint i, FILE *file);
void readEnlaces(uint i, FILE *file);

#endif // F_UTILS
