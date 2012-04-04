#include <stdio.h>
#include <stdlib.h>


char * dumpToStrFromFILE(FILE * fp){
	char *dumpedStr = NULL ;
	char ch ;
	int cnt = 0;
	rewind( fp ) ;
	while( (ch = fgetc(fp)) != EOF ) {
		cnt ++ ;
		dumpedStr = (char *)realloc(dumpedStr , cnt ) ;
		if(dumpedStr == NULL){
			return NULL ;
		}
		dumpedStr[cnt - 1] = ch ;
	}
	dumpedStr = (char *)realloc(dumpedStr , cnt + 1) ;
	if(dumpedStr == NULL){
		return NULL ;
	}
	dumpedStr[cnt ] = '\0' ;
	return dumpedStr ;
}
