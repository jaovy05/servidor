#include <stdio.h>
#include <arpa/inet.h>

#ifndef F_UTILS

#define F_UTILS

void die(char *s);
int makeSocket(struct sockaddr_in si_me, int port);
void readRoteadores(int i, FILE *file);
void readEnlaces(int i, FILE *file);
inline int cmp(const void *a, const void *b);
Roteador *findById(int id);

#endif // F_UTILS
