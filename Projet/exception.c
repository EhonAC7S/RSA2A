#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile;
	infile = fopen("exception.txt", "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
	if (infile == NULL) {
		printf("Unable to open file.\n"); 
	}else{
		printf("Fichier ouvert avec succès.\n");
		while((read = getline(&line, &len, infile)) != -1){
			char * line2 = malloc(strlen(line)-2);
			strncpy(line2, line, strlen(line)-2);
			if (strstr("vroum&ad_classid=vroum", line2) != NULL){ // écrire à gauche l'URL de la page :)
				printf("Je sais lire !\n");				
				fclose(infile);
				return 0;
			}		
		}		
		fclose(infile);
	}
	return 0;
}
