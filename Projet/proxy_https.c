
#include "utils/socketutil.h"
#include "utils/util.h"
#include "utils/patternSearch.h"

int main(int argc, char const *argv[])
{

	char requete[REQMAX], requestType[ENTETE], hostname[MAXHOST];
	char rep[REPMAX], URL[REQMAX];
	fd_set initSet, desc_set;
	int n, maxfd;
	int webDesc, client;
	struct addrinfo *res = NULL;
	int serverIPv4, serverIPv6;
	int clientList[FD_SETSIZE];
	int webList[FD_SETSIZE];
	
	int m, isPub = 0;
	char* buffOK = "HTTP/1.0 200 OK \r\n\r\n"; //Ack au client, on l'envoie en réponse à une demande de pub
	char* ackHTTPS = "HTTP/1.1 200 Connection established\r\n\r\n"; //Message pour signifier au client que la connection va pouvoir être sécure
	if(argc != 2){
		usage();
		exit(1);
	}

	// socket IPv4 et IPv6
	initServer(&res, argv[1]);
	serverIPv4 = createSocket(res);
	res = res->ai_next;
	serverIPv6 = createSocket(res);
	maxfd = openServer(serverIPv4, serverIPv6);

	//Initialise les descripteurs
	FD_ZERO(&initSet);
	FD_ZERO(&desc_set);
	FD_SET(serverIPv4, &initSet);
	FD_SET(serverIPv6, &initSet);

	//On initialise les tableaux de descripteurs
	int i;
	for(i=0; i<FD_SETSIZE-1; i++){
		clientList[i] = -1;
		webList[i] = -1;
	}

	showMyIp(res, argv[1]);
	freeaddrinfo(res);
	
	while(1){

		desc_set = initSet;

		// On fait un appel à select pour voir les activité des descipteurs
		n = select(maxfd, &desc_set, NULL, NULL, NULL);

		/******* Arrivée d'un client *******/
		i = 0;

		// client en IPv4
		if(FD_ISSET(serverIPv4, &desc_set)){
			printf("======== Nouveau client IPv4 ========\n");
			client = addClient(serverIPv4, &initSet);
			if(client >= maxfd) {
				maxfd=client+1;
			}
			//Sauvegarde dans le tableau
			i=0;
			while(clientList[i] >= 0) {
				i++;
			}
			clientList[i] = client;
			n--;
			if(client >= maxfd) {
				maxfd = client +1;
			}
			FD_SET(client, &initSet);
		}

		if(FD_ISSET(serverIPv6, &desc_set))
		{ 
			printf("======== Nouveau client IPv6 ========\n");

			client = addClient(serverIPv6, &initSet);

			if(client >= maxfd) {
				maxfd=client+1;
			}
			i=0;
			while(clientList[i] >= 0)i++;
			clientList[i] = client;
			n--;

			if(client >= maxfd) maxfd = client +1;
			FD_SET(client, &initSet);
		}

		//On vérifie qu'on a pas atteint le nombre maximal de client
		if(i == FD_SETSIZE)
		{
			perror("======== Nombre maximal de clients atteints ========");
			exit(3);
		}

		i = 0;
		while(n > 0 && i<FD_SETSIZE){

			/*** Réponse du serveur web? ***/
			webDesc = webList[i];
			if(webDesc >= 0 && FD_ISSET(webDesc, &desc_set))
			{
				memset(rep, 0, REPMAX);
				m = recv(webDesc, rep, REPMAX, 0);
				if(m < 0){
					perror("Erreur dans la lecture de la reponse");
		  			close(webDesc);
		  			exit(5);
				}else if(m == 0){
					close(webDesc);
					FD_CLR(webDesc, &initSet);
					webDesc = -1;
					printf("======== Connexion avec le serveur web fermée ========\n");
				}else{
					send(clientList[i], rep, m, 0);
				}
				webList[i] = webDesc;
				n--;
			}

			// Requete du client
			client = clientList[i];
			if(client >= 0 && FD_ISSET(client, &desc_set))
			{
				memset(requete, 0, REQMAX);
				m = recv(client, requete, REQMAX, 0);

				if(m < 0){
					perror("Erreur lecture");
		  			close(client);
		  			exit(5);
				}else if(m == 0) { //Le client ferme la connexion

					//On ferme la socket client
					close(client);
					FD_CLR(client, &initSet);
					client = -1;
					
					printf("======== Connexion avec le client fermée ========\n");
					


				}else{
					//On récupère le type de la requête
					searchTypeRequest(requete, requestType);
					//On ne considère que les requêtes GET
					if(strcmp(requestType, "GET") == 0){
						searchHostName(requete, hostname);
						printf("-----------------------------------------------\n");
						searchURL(requete, URL);
						printf("Requete vers : \n%s\n", URL);
						printf("-----------------------------------------------\n");
						isPub = searchPatternIn(URL);

						if (isPub) {
							send(client,buffOK, strlen(buffOK), 0);
							close(client);
							FD_CLR(client, &initSet);
							client = -1;
							printf("======== Blocage d'une demande supposée malveillante ========\n");
						}
						else {
							webDesc = createWebSocket(hostname, "80"); //Port 80 pour aller sur le web
							webList[i] = webDesc; // On créé la web socket sur le même i que son client pour les retrouver

							if(webDesc >= maxfd) {
								maxfd = webDesc +1;
							}
							FD_SET(webDesc, &initSet);

							//Transmission de la requete du client au serveur
							send(webDesc, requete, m, 0);
						}
					}else if(strcmp(requestType, "CLOSE")){
						close(client);
						FD_CLR(client, &initSet);
						client = -1;
						printf("======== Connexion avec le client fermée ========\n");
					} else if (strcmp(requestType, "CONNECT"))
					{
						searchHostName(requete, hostname);
						printf("-----------------------------------------------\n");
						searchURL(requete, URL);
						printf("Requete https vers : \n%s\n", URL);
						printf("-----------------------------------------------\n");
						isPub = searchPatternIn(URL);
						if (isPub) {
							send(client,buffOK, strlen(buffOK), 0);
							close(client);
							FD_CLR(client, &initSet);
							client = -1;
							printf("======== Blocage d'une demande supposée malveillante ========\n");
						}
						else {
							webDesc = createWebSocket(hostname, "443"); //Port 443 pour aller sur le web sécurisé
							webList[i] = webDesc; // On créé la web socket sur le même i que son client pour les retrouver
							send(webDesc, requete, m, 0);
							send(client,ackHTTPS,strlen(ackHTTPS),0);
							webList[i] = webDesc; // On créé la web socket sur le même i que son client pour les retrouver
							if(webDesc >= maxfd) {
								maxfd = webDesc +1;
							}
							FD_SET(webDesc, &initSet);

							//Transmission de la requete du client au serveur
							
						}
					}
				}

				n--;
				clientList[i] = client;
			}

			i++;
		}
	}

	close(serverIPv4);
	close(serverIPv6);
	close(client);

	return 0;
}