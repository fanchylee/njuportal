#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <unistd.h>

#include <sys/types.h>
#include <pwd.h>

#include "definations.h"

#define MAX_USER_NAME_LEN 20
#define MAX_PASSWORD_LEN 100
#define MAX_RECORDFILENAME_LEN 500


#define RELEASE

char user[MAX_USER_NAME_LEN] = "b091180066" ;
char password[MAX_PASSWORD_LEN] = "lpc/1991" ;
char recordfilename[MAX_RECORDFILENAME_LEN] = "/home/fanchylee/.portal.record" ;
	

int perform(int option) ;
extern char * url_encode(char * ) ;
extern char * dumpToStrFromFILE(FILE*) ;
extern int freeDempedStr(char *);
extern int inforecord(enum portal_option , const char * );
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
		return OK_IN_USERFILEREAD  ;
	}else{
		perror("cannot open user file") ;
		exit(EXIT_FAILURE);
	}
}

int perform(int option)
{
	CURL *curl;
	CURLcode res;
	char data[1000] = "action=login&url=http\%3A\%2F\%2Fp.nju.edu.cn&login_username=" ;
	char postdata[] = "&x=29&y=17";
	char *url ;
	struct curl_slist *headers=NULL;   
	FILE* FilePtr;
	FILE* debug = stderr  ;
	
	curl = curl_easy_init();
	if((FilePtr = fopen("/dev/null"  , "w")) == NULL ){
		fprintf(stderr,"cannot open file /dev/null   \n") ;
	}else{
#ifdef RELEASE	
		debug = FilePtr ;
#endif
	switch(option){
		case login:
		url = "http://p.nju.edu.cn/portal/" ;
		strcat(data , url_encode(user)) ;
		strcat(data , "&login_password=") ;
		strcat(data , url_encode(password)) ;
		strcat(data , postdata ) ;
		fputc('\n', debug);
		fputs(data, debug);
		break ;
		
		case disconnect:
		url = "http://p.nju.edu.cn/portal/index.html" ;
		strcpy(data , "action=disconnect") ;
		break ;
		
		default :
		perror("switch error");
		exit(1) ;
		break ;
	}

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, FilePtr);
		curl_easy_setopt(curl, CURLOPT_VERBOSE , 1) ;
#ifdef RELEASE
		curl_easy_setopt(curl, CURLOPT_VERBOSE , 0) ;
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS , 0) ;
#endif
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data); 

		headers = curl_slist_append(headers, "Host: p.nju.edu.cn");
		headers = curl_slist_append(headers, "Connection: keep-alive");
		headers = curl_slist_append(headers, "Referer: http://p.nju.edu.cn/portal/");
		headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
	
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 

		res = curl_easy_perform(curl);
		

		/* always cleanup */
		curl_slist_free_all(headers); /* free custom header list */
		curl_easy_cleanup(curl);
	}
	fclose(FilePtr);
	}
	return 0;
}
int main(int argc, char *argv[]){
	char* option = NULL ;
	
	const char rcname[] = "/.portal" ;
	const char recordname[] = "/.portal.record" ;

	char * userfilename = NULL ;

	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;

	if(argc > 1 ){
		option = argv[1] ;
	}else{
		option = "i" ;
	}	
	
	*recordfilename = '\0' ;
	strcat(recordfilename , homedir) ;
	strcat(recordfilename , recordname) ;

/*login process*/ 
	if(strcmp(option,"l") == 0){
	
	userfilename = malloc(strlen(homedir) + strlen(rcname) + 2) ;
	*userfilename = '\0' ;
	userfilename = strcat(userfilename , homedir);
	userfilename = strcat(userfilename , rcname) ;


	if( access(userfilename,  R_OK) == 0) {
/* will change user[] and password[] */
		userfileread(userfilename) ;	
	}else{if(argc == 4){
		strcpy(user , argv[2]) ;
		strcpy(password, argv[3]) ;
	}else{if(argc == 5){
		strcpy(user , argv[2]) ;
		strcpy(password, argv[3]) ;
		strcpy(recordfilename , argv[4]) ;
	}else{
		perror("请指定用户名和密码,或者创建含有用户名和密码的文件 ~/.portal\n");
		exit(1);
	}}}

	perform(login);
	inforecord(login , recordfilename) ;
/*login process end*/
	}else{if(strcmp(option,"d") == 0){
/*disconnect process start*/
	if(access(userfilename , R_OK) == 0 ) {
		if(userfileread(userfilename) == 
		NO_RECORDFILENAME_IN_USERFILEREAD){
			if(argc == 3 ){
				strcpy(recordfilename , argv[2]) ;
			}else{ if(argc > 3) {
				perror("wrong command amount");
				exit(EXIT_FAILURE);
			}}
		}
	}
	perform(disconnect);
	inforecord(disconnect , recordfilename) ;
/*disconnect process end*/
	}else{if(strcmp(option , "i")){
		/* TODO info */
	}else{
		perror("unknown action");
		exit(1);
	}}}
	return 0 ;

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
