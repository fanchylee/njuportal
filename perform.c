#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definations.h"
#include "globals.h"

#define IP


extern char * url_encode(char * ) ;

int perform(enum portal_option option ,FILE* out ){
	CURL *curl;
	CURLcode res;
#ifndef IP
	char data[1000] = "action=login&url=http\%3A\%2F\%2Fp.nju.edu.cn&p_login=p_login&username=" ;
#else
	char data[1000] = "action=login&url=http\%3A\%2F\%2F219.219.114.15&p_login=p_login&username=" ;
#endif
	char postdata[] = "&x=29&y=17";
	char *url ;
	struct curl_slist *headers=NULL;   

	char* user_encode ;
	char* password_encode ;
	
	curl = curl_easy_init();
	switch(option){
		case login:
#ifndef IP
		url = "http://p.nju.edu.cn/portal.do" ;
#else
		url = "http://219.219.114.15/portal.do" ;
#endif
		strcat(data , user_encode = url_encode(user)) ;
		free(user_encode);
		strcat(data , "&password=") ;
		strcat(data , password_encode = url_encode(password)) ;
		free(password_encode);
		strcat(data , postdata ) ;
#ifndef RELEASE
		fputc('\n', debug);
		fputs(data, debug);
#endif
		break ;
		
		case disconnect:
#ifndef IP
		url = "http://p.nju.edu.cn/portal.do" ;
#else
		url = "http://219.219.114.15/portal.do" ;
#endif
		strcpy(data , "p_logout=p_logout&action=logout") ;
		break ;
		
		case status:
#ifndef IP
		url = "http://p.nju.edu.cn/portal.do" ;
#else
		url = "http://219.219.114.15/portal.do" ;
#endif
		break ;
		
		default :
		perror("switch error");
		exit(1) ;
		break ;
	}

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
#ifdef RELEASE
		curl_easy_setopt(curl, CURLOPT_VERBOSE , 0) ;
#endif
#ifndef  RELEASE
//		curl_easy_setopt(curl, CURLOPT_VERBOSE , 1) ;
#endif
#ifdef RELEASE
//		curl_easy_setopt(curl, CURLOPT_NOPROGRESS , 0) ;
#endif
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data); 
#ifndef IP
		headers = curl_slist_append(headers, "Host: p.nju.edu.cn");
		headers = curl_slist_append(headers, "Referer: http://p.nju.edu.cn/portal.do");
#else
		headers = curl_slist_append(headers, "Host: 219.219.114.15");
		headers = curl_slist_append(headers, "Referer: http://219.219.114.15/portal.do");
#endif
		headers = curl_slist_append(headers, "Connection: keep-alive");
		headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
	
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 

		res = curl_easy_perform(curl);
		

		/* always cleanup */
		curl_slist_free_all(headers); /* free custom header list */
		curl_easy_cleanup(curl);
	}
	return 0;
}
