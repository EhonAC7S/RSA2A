#include "socketutil.h"
#include "util.h"

int createSocket(struct addrinfo *res){
	int serverSocket;

	if ((serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <0) {
		perror("Erreur dans l'ouverture dans la socket\n");
		exit (2);
	}

	unsigned int on = 1;

	//Pour que les deux sockets serveurs puissent se connecter au même port
	if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0){
		perror("Problème dans le setsockopt pour les ports multiples");
		exit(10);
	}
	if (bind(serverSocket,res->ai_addr, res->ai_addrlen) <0) {
		perror ("Erreur dans le bind de la socket\n");
		exit (3);
	}

	return serverSocket;
}

void initServer(struct addrinfo **res, const char *num_port){
	int err_code;
	struct addrinfo criteres;

	//On initialise les criteres
	memset(&criteres, 0, sizeof(criteres));
	//socket serveur
	criteres.ai_flags = AI_PASSIVE;
	//TCP
	criteres.ai_socktype = SOCK_STREAM;
	//IPv4 ou IPv6
	criteres.ai_family = AF_UNSPEC;
	//On met le node à NULL pour avoir un socket de serveur
	getaddrinfo(NULL, num_port, &criteres, res);
	if(getaddrinfo(NULL, num_port, &criteres, res)){
		perror("Erreur dans le getaddreinfo \n");
		exit(1);
	}
}

int openServer(int serverSocketIPv4, int serverSocketIPv6){
	if (listen(serverSocketIPv6, SOMAXCONN) <0) {
		perror ("Erreur listen IPv6\n");
		exit (4);
	}
	if (listen(serverSocketIPv4, SOMAXCONN) <0) {
		perror ("Erreur listen IPv4\n");
		exit (4);
	}
	
	if (serverSocketIPv4 < serverSocketIPv6)
	{
		return serverSocketIPv6 + 1;
	} else 
	{
		return serverSocketIPv4 + 1;
	}
	
}

int createWebSocket(char hostname[], char portSortie[])
{
	//Pour gérer getaddrinfo
	
	struct addrinfo *res, base;
	int webSocket;

	//On initialise les criteres
	memset(&base, 0, sizeof(base));

	//On veut du TCP
	base.ai_socktype = SOCK_STREAM;

	//On veut tout prendre : IPv4 et IPv6
	base.ai_family = AF_UNSPEC;

	if(getaddrinfo(hostname, portSortie, &base, &res))
	{
		perror("Erreur dans le getaddreinfo\n");
		exit(1);
	}

	if ((webSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <0) 
	{
		perror("Erreur dans l'ouverture dans la socket\n");
		exit (2);
	}

	unsigned int on = 1;

	//Pour que les deux sockets serveurs puissent se connecter au même port
	if(setsockopt(webSocket, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0)
	{
		perror("Problème dans le setsockopt pour les ports multiples");
		exit(10);
	}
	if(connect (webSocket, res->ai_addr, res->ai_addrlen)  < 0)
	{
		printf("Problème connect pour la socket web\n");
		exit(1);
	}
	freeaddrinfo(res);
	return webSocket;
} 

int addClient(int socket, fd_set *set)
{
	socklen_t clilen;
	struct sockaddr_in6 cli_addr;
	int sockCli;

	clilen = sizeof(cli_addr);
	sockCli = accept(socket, (struct sockaddr *) &cli_addr, &clilen);

	if (sockCli < 0) 
	{
		perror("Il y a eu une erreur dans la création de la socket d'écoute du nouveau client\n");
		exit (1);
	}

	//On ajoute le client aux descripteurs à suivre
	FD_SET(sockCli, set);

	char addr[150];
	inet_ntop(cli_addr.sin6_family, cli_addr.sin6_addr.s6_addr, addr, sizeof(cli_addr.sin6_addr.s6_addr));
	printf("Le client d'adresse %s et de port %d a bien été ajouté\n", addr, cli_addr.sin6_port);

	return sockCli;
}