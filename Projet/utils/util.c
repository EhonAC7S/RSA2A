#include "util.h"

void usage(){
  perror("Il faut renseigner le num�ro de port pour les socket !\n");
}

void searchTypeRequest(char entete [], char type[]){
  int i = 0;

  //On r�cup�re la commande
  memset(type, 0, MAXENTETE);
  while(entete[i] != ' ' && i < MAXENTETE){
    type[i] = entete[i];
    i++;
  }
}

void searchHostName(char entete[], char hostname[]){
  int i=0;
  int https = 0;

  //On passe la commande
  while(entete[i] != ' ' && i < MAXENTETE)i++;

  //On r�cup�re le hostname
  int newi = ++i; //On supprime l'espace
  int hosti = 0;

  //On regarde si'il y a http://
  char http[8];
  while(i - newi < 7){
    http[i - newi] = entete[i];
    i++;
  }
  http[7] = '\0';

  //On �limine le http:// s'il est pr�sent
  if(!strcmp(http, "http://")){
    newi = i;
  }else{
    https = 1;
  }

  //Le d�limitateur d�pend : si on a HTTP, c'est /. Si on a HTTPS, c'est :
  char limit = (https) ? ':' : '/';

  while(entete[newi] != limit && i < MAXHOST){
    hostname[hosti++] = entete[newi];
    newi++;
  }

  hostname[hosti]='\0';
}

void searchRequest(char entete[], char requete[]){
  int i=0;

  //On passe la commande
  while(entete[i] != ' ' && i < MAXENTETE)i++;

  int newi = ++i;
  int reqi = 0;

  //On regarde si'il y a http://
  char http[8];
  while(i - newi < 7){
    http[i - newi] = entete[i];
    i++;
  }
  http[7] = '\0';

  //On �limine le http:// s'il est pr�sent
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

void addRequestLog(int socketClient, char type_req[], char request[]){
  FILE *log_requests;

  //On r�cup�re les infos sur le client � partir du fd
  struct sockaddr_in6 addr;
  socklen_t addr_size = sizeof(struct sockaddr_in);
  int res = getpeername(socketClient, (struct sockaddr*)&addr, &addr_size);

  char ip[150];
  inet_ntop(addr.sin6_family, addr.sin6_addr.s6_addr, ip, sizeof(addr.sin6_addr.s6_addr));

  if((log_requests = fopen("./logs/log_requests", "a+")) == NULL){
    perror("Erreur lors de l'ouverture du fichier logs/log_requests");
    exit(1);
  }

  fprintf(log_requests, "%s %d %s %s\n", ip, addr.sin6_port, type_req, request);

  if(fclose(log_requests) != 0){
    perror("Erreur lors de la fermeture de logs/log_requests");
  }
}

//void searchURL(char entete[], char uRL[]) {


//}



