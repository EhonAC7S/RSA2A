#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int contient(char * chaine, char * strArray[], int i){
    int resultat = 0;
	char * ret;
	for (int j=0;j<i;j++){
		if((ret = strstr(chaine, strArray[j])) != NULL){
			resultat++;
			chaine = ret;
		}
	}if (resultat == i){
		return 1;
	}else{
		return -1;
	}
}



int main(int argc, char **argv){
	FILE *infile;
	infile = fopen("MignonnesExceptions.txt", "r");
	char * line = NULL;
	char * line2 = NULL;
	char * strArray[10];
    char * chaine = "quiveutdeschat";
	char * token = NULL;
    size_t len = 0;
    ssize_t read;
    
	if (infile == NULL) {
		printf("Erreur à l'ouverture du fichier.\n"); 
	
	}else{
		printf("Fichier ouvert avec succès.\n");
		
		//On commence à lire le fichier d'exception.
		while(((read = getline(&line, &len, infile)) != -1)){
			//on retire le \n de la ligne lue.
			//printf("hey\n");
			line2 = calloc(1,strlen(line)-1);
			strncpy(line2, line, strlen(line)-1);
			//printf("%s\n", line2);
			//Si la ligne contient des étoiles 
			if(strchr(line2, '*')!=NULL){
				int i = 0;
				token = strtok(line2, "*");
				while( token != NULL) {
					printf("%s", token );
					strArray[i] = malloc(strlen(token));
					strcpy(strArray[i], token);
					token = strtok(NULL, "*");
					i++;
				}
				free(line2);
				printf("\n");
				if (contient(chaine, strArray,i)){
					printf("it works\n");
				}
			
			//free de notre tableau de string 
				for (int j=0;j<i;j++){
					free(strArray[j]);
				}
			}			
		}		
		fclose(infile);
	}
	return 0;
}


			
			//on ne lit pas les commentaires
				/*else{
					printf("Bloqué %s %s\n",line, chaine);				
					fclose(infile);
					free(line2);
					return 0;
			else if(strncmp(line,"@@",2)==0){
				printf("%s\n",line);
				}
				}*/		
