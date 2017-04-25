#include "util.h"

void usage(){
  perror("Il faut renseigner le numéro de port pour les socket !\n");
}

void searchTypeRequest(char entete [], char type[]){
  int i = 0;

  //On récupère la commande
  memset(type, 0, ENTETE);
  while(entete[i] != ' ' && i < MAXENTETE){
    type[i] = entete[i];
    i++;
  }
}

void searchURL(char entete[], char URL[]){
  int i = 0;
  while(entete[i] != ' '){
	  i++;
	  }
  int newi = i+1; 
  int url = 0;   
  
  while(entete[newi] != ' '){
    URL[url++] = entete[newi];
    newi++;
  }
  URL[url]='\0';
}

void searchHostName(char entete[], char hostname[])
{
  int i=0;
  int https = 0;
  while(entete[i] != ' ' && i < ENTETE) 
  {
    i++;
  }
  int newi = ++i; 
  int hosti = 0;
  
  char http[8];
  while(i - newi < 7){
    http[i - newi] = entete[i];
    i++;
  }
  http[7] = '\0';

  if(!strcmp(http, "http://")){
    newi = i;
  }else{
    https = 1;
  }

  char limit = (https) ? ':' : '/';

  while(entete[newi] != limit && i < MAXHOST)
  {
    hostname[hosti++] = entete[newi];
    newi++;
  }

  hostname[hosti]='\0';
}

void searchRequest(char entete[], char requete[]){
  int i=0;

  //On passe la commande
  while(entete[i] != ' ' && i < ENTETE)i++;

  int newi = ++i;
  int reqi = 0;

  //On regarde si'il y a http://
  char http[8];
  while(i - newi < 7){
    http[i - newi] = entete[i];
    i++;
  }
  http[7] = '\0';

  //On élimine le http:// s'il est présent
  if(!strcmp(http, "http://")) newi = i;

  while(entete[newi] != ' ' && i < MAXHOST){
    requete[reqi++] = entete[newi];
    newi++;
  }

  requete[reqi] = '\0';
}

void showMyIp(struct addrinfo *res, const char *port){
  struct sockaddr_in6 *my_addr = (struct sockaddr_in6 *)res->ai_addr;
  char ip[150];
  inet_ntop(my_addr->sin6_family, my_addr->sin6_addr.s6_addr, ip, sizeof(my_addr->sin6_addr.s6_addr));
  printf("\n==================================\n");
  printf("Lancement du serveur sur l'adresse %s sur le port %s \n", ip, port);
  printf("\n==================================\n");
}




