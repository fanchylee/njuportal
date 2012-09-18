#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "definations.h"

#define MAX_USER_NAME_LEN 20
#define MAX_PASSWORD_LEN 100
#define MAX_FILENAME_LEN 500


#define RELEASE

char user[MAX_USER_NAME_LEN] = "b091180066" ;
char password[MAX_PASSWORD_LEN] = "lpc/1991" ;
char recordfilename[MAX_FILENAME_LEN] = "/etc/.portal.record" ;
char userfilename[MAX_FILENAME_LEN] = "/etc/.portal" ;
const char *homedir ;
FILE* debug ;
FILE* trashfile ;
	

extern int perform(enum portal_option option , FILE* out) ;
extern char * dumpToStrFromFILE(FILE*) ;
extern int freeDempedStr(char *);
extern int inforecord(enum portal_option , const char * );
extern int regexmatch(FILE* in, const char* matchstr , FILE* out);
static int userfileread(const char * userfilename);
static char* fieldcpy(char* dest , char* source) ;
static char* omitwhitespace(char* start) ;


#define NO_RECORDFILENAME_IN_USERFILEREAD 1 
#define OK_IN_USERFILEREAD 0

static int userfileread(const char* userfilename){
	FILE *fp = NULL;
	char* uinfo = NULL ;
	char* uinfo_head = NULL ;
	char* temp = NULL ;
	if((fp = fopen(userfilename,"r")) != NULL){
		uinfo_head = dumpToStrFromFILE(fp) ;
		uinfo = uinfo_head ;
		uinfo = fieldcpy(user , uinfo) ;
		uinfo = omitwhitespace(uinfo) ;
		uinfo = fieldcpy(password , uinfo) ;
		uinfo = omitwhitespace(uinfo) ;
		if(*uinfo != '\0' ){
			fieldcpy(recordfilename , uinfo) ;
		}else{
			return NO_RECORDFILENAME_IN_USERFILEREAD ;
		}
		freeDumpedStr(uinfo_head) ;
		fclose(fp) ;
		return OK_IN_USERFILEREAD  ;
	}else{
		perror("cannot open user file") ;
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char *argv[]){
	char* option = NULL ;
	
	const char rcname[] = "/.portal" ;
	const char recordname[] = "/.portal.record" ;
	enum portal_option opt ;
	pid_t pid ;
	int curl_pipe[2] ;
	FILE* curlin ;
	FILE* curlout ;

/*
 * files
 */
	homedir = getenv("HOME");

	*recordfilename = '\0' ;
	strcat(recordfilename , homedir) ;
	strcat(recordfilename , recordname) ;
	*userfilename = '\0' ;
	strcat(userfilename , homedir);
	strcat(userfilename , rcname) ;
#ifndef RELEASE
	debug = stderr ;
#endif
	if((trashfile = fopen("/dev/null"  , "w")) == NULL){
		perror("cannot open file /dev/null") ;
		exit(EXIT_FAILURE);
	}
#ifdef RELEASE	
	debug = trashfile ;
#endif
	if(pipe(curl_pipe) < 0){
		perror("pipe error");
		exit(EXIT_FAILURE);
	}else if((curlin  = fdopen(curl_pipe[1] ,"w")) == NULL){
		perror("fdopen pipe 1 error for write");
		exit(EXIT_FAILURE);
	}else if((curlout = fdopen(curl_pipe[0] ,"r")) == NULL){
		perror("fdopen pipe 0 error for read");
		exit(EXIT_FAILURE);
	}
/*
 * option  
 */	

	if(argc > 1 ){
		option = argv[1] ;
	}else{
		option = "s" ;
	}	

	if(strcmp(option,"l") == 0){
/*login start*/ 
		if( access(userfilename,  R_OK) == 0) {
/* will change user[] and password[] */
			userfileread(userfilename) ;	
		}else if(argc == 4){
			strcpy(user , argv[2]) ;
			strcpy(password, argv[3]) ;
		}else if(argc == 5){
			strcpy(user , argv[2]) ;
			strcpy(password, argv[3]) ;
			strcpy(recordfilename , argv[4]) ;
		}else{
			perror("please specify the username and password or create ~/.portal\n");
			exit(1);
		}
		opt = login ;
/*login  end*/
	}else if(strcmp(option,"d") == 0){
/*disconnect start*/
		if(access(userfilename , R_OK) == 0 ) {
			if(userfileread(userfilename) == 
			NO_RECORDFILENAME_IN_USERFILEREAD){
				if(argc == 3 ){
					strcpy(recordfilename , argv[2]) ;
				}else if(argc > 3) {
					perror("wrong command amount");
					exit(EXIT_FAILURE);
				}
			}
		}
		opt = disconnect ;
/*disconnect end*/
	}else if(strcmp(option , "s") == 0){
		opt = status ;
	}else{
		perror("unknown action");
		exit(EXIT_FAILURE);
	}
/*
 * child process
 */
	if((pid = fork() ) < 0){
		perror("fork error") ;
		exit(EXIT_FAILURE) ;
	}else if(pid > 0){
/*this process start*/	
		fclose(curlin) ;
		switch(opt){
		case login:
		if(regexmatch(curlout, "您已经登录",debug)){
			fprintf(stderr, "\nPortal has been on\n");
			return 0 ;
		}else{
			fprintf(stdout, "\nlogin \n");
		}
		inforecord(opt , recordfilename) ;
		break ;

		case disconnect:
		if(regexmatch(curlout,"下线失败",debug)){
			fprintf(stderr, "\nPortal has been off\n");
			return 0;
		}else{
			fprintf(stdout, "\ndisconnect\n"); 
		}
		inforecord(opt , recordfilename) ;
		break ;

		case status:
		if(regexmatch(curlout, "请输入", debug)){
			fprintf(stdout, "\nnot connected\n");
		}else{
			fprintf(stdout, "\nconnected\n");
		}
		break ;
		
		default:
		fclose(curlout);
		perror("unknown portal option") ;
		exit(EXIT_FAILURE);
		break ;
		}
		fclose(curlout);
/*this process end*/
	}else {
/* child process start*/
		fclose(curlout);
#ifndef RELEASE
		perform(opt , stdout);
#endif
#ifdef RELEASE
		perform(opt, curlin);
#endif
		fclose(curlin) ;
/* child process end*/
	}
	fclose(trashfile);
	return 0;
}


/* copy a field string*/
static char* fieldcpy(char* dest,char* source){
	while((*dest = *source) != '\0'
	&& (*dest = *source) != '\n' 
	&& (*dest = *source) != '\t' 
	&& (*dest = *source) != ' '){
		dest ++ ;
		source ++ ;
	}
	*dest = '\0' ;
	return source ;
}
static char* omitwhitespace(char* start){
	while(*start  == ' '
	|| *start == '\n'
	|| *start == '\t'){
		start++ ;
	}
	return start ;
}
