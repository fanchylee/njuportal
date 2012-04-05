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

#define RELEASE

char user[MAX_USER_NAME_LEN] = "b091180066" ;
char password[MAX_PASSWORD_LEN] = "lpc/1991" ;

	

int perform(int option) ;
extern char * url_encode(char * ) ;
extern char * dumpToStrFromFILE(FILE*) ;
extern int freeDempedStr(char *);
extern int inforecord(enum portal_option , const char * );


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
	FILE *fp = NULL;
	char* uinfo = NULL ;
	char* uinfo_head = NULL ;
	char* temp = NULL ;
	char* option = NULL ;
	
	const char rcname[] = "/.portal" ;
	const char recordname[] = "/.portal.record" ;

	char * userfile = NULL ;
	char * recordfile = NULL ;

	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;

	if(argc > 1 ){
		option = argv[1] ;
	}else{
		option = "i" ;
	}	
	
	recordfile = malloc(strlen(homedir) + strlen(recordname) + 2) ;
	*recordfile = '\0' ;
	recordfile = strcat(recordfile , homedir) ;
	recordfile = strcat(recordfile , recordname) ;

/*login process*/ 
	if(strcmp(option,"l") == 0){
	
	userfile = malloc(strlen(homedir) + strlen(rcname) + 2) ;
	*userfile = '\0' ;
	userfile = strcat(userfile , homedir);
	userfile = strcat(userfile , rcname) ;

	


	if( access(userfile,  R_OK) == 0) {
		if((fp = fopen(userfile,"r")) != NULL){
			uinfo_head = dumpToStrFromFILE(fp) ;
			uinfo = uinfo_head ;
			temp = user ;
			while((*temp = *uinfo) != '\0'
				&& (*temp = *uinfo) != '\n' 
				&& (*temp = *uinfo) != '\t' 
				&& (*temp = *uinfo) != ' '){
				temp++;
				uinfo++;
			}
			*temp = '\0' ;
			while(*uinfo  == ' '
				|| *uinfo == '\n'
				|| *uinfo == '\t')uinfo++ ;
			temp = password ;
			while((*temp = *uinfo) != '\0'
				&& (*temp = *uinfo) != '\n' 
				&& (*temp = *uinfo) != '\t' 
				&& (*temp = *uinfo) != ' '){
				temp ++ ;
				uinfo ++ ;
			}
			*temp = '\0' ;
			freeDumpedStr(uinfo_head) ;
		}else{
			perror("cannot open user file") ;
			exit(1);
		}
	}else{if(argc == 4){
		strcpy(user , argv[2]) ;
		strcpy(password, argv[3]) ;
	}else{if(argc == 5){
		free(recordfile);
		recordfile = malloc(strlen(argv[4])+2);
		recordfile = strcpy(recordfile , argv[4]) ;
	}else{
		perror("请指定用户名和密码,或者创建含有用户名和密码的文件 ~/.portal\n");
		exit(1);
	}}}

	perform(login);
	inforecord(login , recordfile) ;
/*login process end*/
	}else{if(strcmp(option,"d") == 0){
/*disconnect process start*/
	if(argc == 2){
	}else{if(argc == 3){
		free(recordfile);
		recordfile = malloc(strlen(argv[2])+2);
		recordfile = strcpy(recordfile , argv[2]) ;
	}else{
		perror("wrong command amount");
		exit(EXIT_FAILURE);
	}}
	perform(disconnect);
	inforecord(disconnect , recordfile) ;
/*disconnect process end*/
	}else{
		perror("unknown action");
		exit(1);
	}}
	return 0 ;

}
