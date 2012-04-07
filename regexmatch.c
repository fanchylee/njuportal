#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <regex.h>

#include "globals.h"

#define MAX_SEARCH_LEN 500


static char toBeSearched[MAX_SEARCH_LEN] ;

int regexmatch(FILE* in, const char * pattern, FILE* out) {
	regex_t  reg ;
	regmatch_t pmatch[1] ;
	char * matchStr = NULL ;
	int err ;
	char errbuf[128]; 
	
	while(fgets(toBeSearched,MAX_SEARCH_LEN,in) != NULL){
	if(err = regcomp(&reg, pattern , 0) ){ 
		regerror (err, &reg, errbuf, sizeof (errbuf)); 
		fprintf (stderr, "error: regcomp: %s\n", errbuf); 
		perror("regex error");
		regfree(&reg);
		exit(EXIT_FAILURE);
	} 

	if ((err = regexec(&reg, toBeSearched , 1 , pmatch ,0 ) )
	== REG_NOMATCH) { 
#ifndef RELEASE
		fprintf (debug, "Sorry, no match ...\n"); 
#endif
		continue;
	} else if(err){ 
		regerror (err, &reg, errbuf, sizeof(errbuf)); 
		fprintf (stderr, "error: regexec: %s\n", errbuf); 
		perror("regex error");
		regfree(&reg);
		exit(EXIT_FAILURE);
	} else if( (matchStr = (char *)realloc(matchStr , pmatch->rm_eo - pmatch->rm_so + 10) )== NULL) {
#ifndef RELEASE
		fprintf(debug , "cannot allocate memory \n");
#endif
		goto exeerr;
	} else {
		strncpy(matchStr, toBeSearched + pmatch->rm_so, pmatch->rm_eo - pmatch->rm_so ); 
		matchStr[pmatch->rm_eo - pmatch->rm_so] = '\0' ;
		if(out != NULL){
			fprintf(out ,"%s\n", matchStr ) ;
		}
		free(matchStr) ;
	}
	regfree(&reg) ;
	return 1 ;
	}
exeerr:	regfree(&reg); 
	return 0 ;
}
/*
int main(int argc , char *argv[]){
	if(regexmatch(stdin,argv[1],stdout) != NULL) {
		fprintf(stdout,"matched\n");
	}else {
		fprintf(stdout,"unmatched\n");
	}
}
*/
