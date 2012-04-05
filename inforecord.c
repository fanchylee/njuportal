#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "definations.h"
#include "globals.h"

int inforecord(enum portal_option o){
	time_t now ;
	FILE* record ;
	char opt ;

	if((record = fopen(recordfilename  , "a")) == NULL ){
		perror("cannot open record file in $HOME dir\n") ;
		exit(1);
	}
	now = time(0);

	switch(o){
		case login:opt = 'l' ;break;
		case disconnect:opt = 'd' ;break ;
		default:
		perror("unknown error\n"  ) ;
		exit(1) ;
	}
	fprintf(record , "%c @%ld\n" , opt , now) ;
	return 0  ;
}
