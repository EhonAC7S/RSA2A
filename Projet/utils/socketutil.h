#ifndef SOCKETUTILS
#define SOCKETUTILS

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>


int createSocket(struct addrinfo *res);
int createWebSocket(char hostname[], char portSortie[]);
int addClient(int socket, fd_set *set);
int openServer(int serverSocket4, int serverSocket6);
void initServer(struct addrinfo **res, const char *num_port);

#endif