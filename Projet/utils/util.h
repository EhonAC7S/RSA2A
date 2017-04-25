#ifndef UTILS
#define UTILS

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
#include <stdlib.h>
#include <stdio.h>

#include "socketutil.h"

#define REQMAX 5000
#define MAXBUFF 1000
#define ENTETE 50
#define MAXHOST 100
#define REPMAX 1500

void showMyIp(struct addrinfo *res, const char *port);
void searchURL(char entete[], char URL[]);
void usage();
void searchTypeRequest(char entete [], char type[]);
void searchHostName(char entete[], char hostname[]);
void searchRequest(char entete[], char requete[]);

#endif
