#include "patternSearch.h"
#include "socketutil.h"
#include "util.h"

int contient(char * chaine, char * strArray[], int i)
{
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
		return 0;
	}
}

int beginWithDoubleBar(char* line) 
{
	if (*line == '|' && *(line+1) == '|') {
		return 1;
	}
	else return 0;
}


int starExprCase(char* chaine, char * line2) 
{
	char * token = NULL;
	int returnValue;	
	int k = 0;
	char * strArray[10];
	//Si la ligne contient des étoiles 
	if(strchr(line2, '*')!=NULL)
	{
		
		token = strtok(line2, "*");
		while( token != NULL) 
		{
			

			strArray[k] = malloc(strlen(token));
			strcpy(strArray[k], token);
			token = strtok(NULL, "*");
			k++;
		}
		//free(line2);
		// On cherche si les éléments parsés se retrouvent dans chaine
		if (contient(chaine, strArray,k))
		{
			returnValue = 1; // On a trouvé ce motif dans chaine
		}else
		{
			returnValue = 0; // On a pas trouvé ce motif dans chaine
		}
		//free de notre tableau de string 
		/*for (int j=0;j<k;j++)
		{
			free(strArray[j]);
		}
		return returnValue;*/
	}
	else { // On a pas d'étoile est donc on considère l'expression
		strArray[0] = malloc(strlen(line2));
		strcpy(strArray[0], line2);
		if (contient(chaine, strArray,1))
		{
			//printf("Motif reconnu\n");
			returnValue = 1;	// On a trouvé ce motif dans chaine
		}else
		{
			returnValue = 0; //On a pas trouvé ce motif
		}
		free(strArray[0]);
		return returnValue;
	}
}

int isAnException(char* line) 
{
	if (*line == '@' && *(line+1) == '@') 
	{
		line = line + 2; //On fait deja le décalage
		return 1; //c'est une exceptions à la liste de rejet
	}
	return 0; //c'est une regle de rejet classique
}


int searchPatternIn(char* chaine){
	FILE *infile;
	infile = fopen("rejectList.txt", "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
	if (infile == NULL) {
		printf("Unable to open file.\n"); 
	}else{

		while((read = getline(&line, &len, infile)) != -1){
			printf("%s\n", line);
			char * line2 = malloc(strlen(line)-2);
			strncpy(line2, line, strlen(line)-2);
			if (strstr(chaine, line2) != NULL){
				printf("Une exception est détecté %s\n", line2);				
				fclose(infile);
				return 1;
			}
			free(line2);		
		}		
		fclose(infile);
	}
	return 0;
}



