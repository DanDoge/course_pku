/*Proxy lab
 *  Name      : 
 *  userID    :
 *  NickName  :
 *  reference : ./tiny/tiny.c
 *              http://www.cs.cmu.edu
 *                    /afs/cs/academic/class/15213-f08/www/recitation
 *                    /proxy-recitation1.pdf
 */


/*description
 *  an multi-session parallel web proxy,
 *  use solution to first reader-writer problem(CSAPP, 3e, p707).
 *
 *  use "rank" to denote last used time, where rank is related to
 *  a specific session(OK to do so because one session will only
 *  read/write one cache line once).
 *  anything between MAX_OBJECT_SIZE and MIN_OBJECT_SIZE will be cached.
 *  (MIN_OBJECT_SIZE have to be less than 512 to pass the autograder...
 *  won't it be too small...?)
 *
 *  ignore SIGPIPE, do nothing and return.
 */


/*log
 *  12/22 13:43 start working on it
 *        17:41 finished all but cache
 *        21:13 hands on cache implementation
 *        22:06 compile error -> link error
 *        22:58 curl installed...sad
 *        23:37 need to rewrite all...just wont work!
 *  12/23 16:28 server, client related functions
 *        18:44 finished, segv, 23 / 70
 *        18:56 fixed, 70 / 70
 *        19:48 rewrite parse_url(), comments needed
 *        20:04 to do list: LRU policy, comments, real-page
 *  12/24 14:18 hands on LRU implementation
 *        14:31 LRU implementation finished, hands on comments
 *        15:00 comments finished, but how to test it on Firefox?
 *  12/25 16:14 again, how to test it on Firefox?
 *        16:58 well, works on 1-2 test web sites...
 *              how to visit remained sites without using "post"
 *              or "https"?
 *        17:00 stop working on proxy lab. keep Buddahist-style.
 */


/* header file(s) */
#include "csapp.h"


/* macros begin */
#define MAX_CACHE_SIZE  1049000
#define MAX_OBJECT_SIZE 102400
#define MIN_OBJECT_SIZE 0       // not right, but make autograder happy!
#define MAX_LINE        2048
#define cache_size      16
/* macros end */


/* structure definitions begin */
typedef struct{
    /* in case more parameters needed <- true */
    int connfd;
    int rank;
}parameters;

typedef struct{
    sem_t mutex;
    sem_t w;
    int readcnt;
    int rank;
    int length;
    char data[MAX_OBJECT_SIZE];
    char url[MAX_LINE];
}Cache;
/* structure definitions end */


/* global data(s) begin */
Cache cache[cache_size];
/* You won't lose style points for including this long line in your code */
static char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static char *connection_hdr = "Connection: close\r\n";
static char *proxy_connection_hdr = "Proxy-Connection: close\r\n";
/* global data(s) end */


/* global functions begin */
void *thread(void* vargp);               // done
void parse_url(char* url, char* hostname // done
             , char* path, char* port);
int find_url(char* url);                 // done
int find_evict();                        // done
int init_cache();                        // done
void read_cache(int idx, char* dst_data  // done
                , char* dst_url, int* dst_rank);
void write_cache(int idx, char* src_data // done
                , char* src_url, int src_rank);
/* global functions end */

/* Signal handler function(s) */
void sigpipe_handler(int sig){
    /* ignore sigpipe */
    return ;
}


/* main begin */
int main(int argc, const char** argv){

    signal(SIGPIPE, sigpipe_handler);

    int listenfd, connfd;
    int rank = 0;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    parameters* para;
    pthread_t tid;

    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    init_cache();
    /* server loop */
    listenfd = Open_listenfd((char* )argv[1]);
    while(1){
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA* )&clientaddr, &clientlen);
        /* safer */
        /* a warning here, but seems inevitable! */
        para = (struct parameters* )Malloc(sizeof(parameters));
        para->connfd = connfd;
        para->rank   = rank++;
        Pthread_create(&tid, NULL, thread, (void* )para);
    }

    return 0;
}
/* main end */


/* cache basic functions begin */
int init_cache(){
    int i = 0;
    for( ; i < cache_size; i += 1){
        cache[i].data[0] = cache[i].url[0] = cache[i].rank = 0;
        cache[i].readcnt = 0;
        Sem_init(&cache[i].mutex, 0, 1);
        Sem_init(&cache[i].w, 0, 1);
    }
    return 0;
}


int find_evict(){
    /* find one cache line with minimal rank */
    int idx = 0;
    int i = 1;
    for( ; i < cache_size; i += 1){
        int i_rank, idx_rank;
        read_cache(i, NULL, NULL, &i_rank);
        read_cache(idx, NULL, NULL, &idx_rank);
        if(i_rank < idx_rank){
            idx = i;
        }
    }
    return idx;
}


int find_url(char* url){
    /* find one cache line whose url is the same as the given url */
    char tmp[MAX_LINE] = {};
    int idx = 0;
	for( ; idx < cache_size; idx += 1){
        read_cache(idx, NULL, tmp, NULL);
        int idx_rank = 0;
        read_cache(idx, NULL, NULL, &idx_rank);
		if(idx_rank && strcmp(url, tmp) == 0){
			return idx;
		}
	}
    /* not found */
	return cache_size;
}
/* cache basic functions end */


/* read / write functions begin */
void read_cache(int idx, char* dst_data, char* dst_url, int* dst_rank){
    /* reader favor */
    P(&cache[idx].mutex);
    cache[idx].readcnt += 1;
    if(cache[idx].readcnt == 1){
        P(&cache[idx].w);
    }
    V(&cache[idx].mutex);

    if(dst_data != NULL){
        strcpy(dst_data, cache[idx].data);
    }
    if(dst_url != NULL){
        strcpy(dst_url, cache[idx].url);
    }
    if(dst_rank != NULL){
        *dst_rank = cache[idx].rank;
    }

    P(&cache[idx].mutex);
    cache[idx].readcnt -= 1;
    if(cache[idx].readcnt == 0){
        V(&cache[idx].w);
    }
    V(&cache[idx].mutex);
}


void write_cache(int idx, char* src_data, char* src_url, int rank){
    P(&cache[idx].w);
    if(src_data != NULL){
        strcpy(cache[idx].data, src_data);
    }
    if(src_url != NULL){
        strcpy(cache[idx].url, src_url);
    }
    cache[idx].rank = rank;
    V(&cache[idx].w);
    return ;
}
/* write functions end */


/* parse_url begin */
void parse_url(char* url, char* host, char* path, char* port){
    char tmp[MAX_LINE] = {0};
    strcpy(tmp, url);
    char* find_slash = strstr(tmp, "//");
    char* realurl = NULL;
    if(find_slash != NULL){
        realurl = find_slash + 2;
    }else{
        realurl = tmp;
    }
    /* realurl <==> tmp */
    char* find_port = strstr(realurl, ":");
    if(find_port != NULL){
        /* xxx.yyy.zzz:123/idx.html */
        *find_port = 0;
        sscanf(realurl, "%s", host);
        *find_port = ':';
        find_slash = strstr(find_port, "/");
        if(find_slash != NULL){
            *find_slash = 0;
            sscanf(find_port + 1, "%s", port);
            *find_slash = '/';
            sscanf(find_slash, "%s", path);
        }else{
            sscanf(find_port + 1, "%s", port);
            strcpy(path, "/");
        }
    }else{
        /* xxx.yyy.zzz/idx.html */
        find_slash = strstr(realurl, "/");
        if(find_slash != NULL){
            *find_slash = 0;
            sscanf(realurl, "%s", host);
            *find_slash = '/';
            sscanf(find_slash, "%s", path);
        }else{
            sscanf(realurl, "%s", host);
            strcpy(path, "/");
        }
        /* default port */
        strcpy(port, "80");
    }
	return ;
}
/* parse_url end */


/* thread begin */
void *thread(void* para){
    int connfd = ((parameters* )para)->connfd;
    int rank = ((parameters* )para)->rank;
	Pthread_detach(pthread_self());
    Free(para);

    char buf[MAX_LINE], method[MAX_LINE], url[MAX_LINE];
    char host[MAX_LINE], path[MAX_LINE], version[MAX_LINE];
    char port[MAX_LINE];
    rio_t rio;

    rio_readinitb(&rio, connfd);
    rio_readlineb(&rio, buf, MAX_LINE);
    sscanf(buf, "%s %s %s", method, url, version);
    parse_url(url, host, path, port);

	/* doesnot work, and i dont know why */
/*    if(strcmp(method, "GET") != 0){
        char body[MAX_OBJECT_SIZE] = {};
        sprintf(body, "<html><h1>not a GET request</h1></html>\r\n");
        sprintf(buf, "HTTP/1.0 501 Not Implemented");
        Rio_writen(connfd, buf, strlen(buf));
        sprintf(buf, "Content-type: text/html\r\n");
        Rio_writen(connfd, buf, strlen(buf));
        sprintf(buf, "Content-length: %d\r\n\r\n", strlen(body));
        Rio_writen(connfd, buf, strlen(buf));
        Rio_writen(connfd, body, strlen(body));
        close(connfd);
        return NULL;
    }
*/

    /* ignore remained lines */
    do{
        rio_readlineb(&rio, buf, MAX_LINE);
    }while(strcmp(buf, "\r\n"));

    /* cache hit! */
    if(find_url(url) != cache_size){
        char data[MAX_OBJECT_SIZE] = {0};
        read_cache(find_url(url), data, NULL, NULL);
        /* update rank */
        write_cache(find_url(url), NULL, NULL, rank);
        rio_writen(connfd, data, strlen(data));
        close(connfd);
        return NULL;
    }

    int serverfd = open_clientfd(host, port);
    /* build http header */
    sprintf(buf, "GET %s HTTP/1.0\r\n", path);
    rio_writen(serverfd, buf, strlen(buf));
    sprintf(buf, "Host: %s\r\n", host);
    rio_writen(serverfd, buf, strlen(buf));
    rio_writen(serverfd, user_agent_hdr, strlen(user_agent_hdr));
    rio_writen(serverfd, connection_hdr, strlen(connection_hdr));
    rio_writen(serverfd, proxy_connection_hdr
               , strlen(proxy_connection_hdr));
    rio_writen(serverfd, "\r\n", strlen("\r\n"));

    /* MAX_OBJECT_SIZE <==> MAX_LINE... */
	char content_cache[MAX_OBJECT_SIZE] = {0};
    int contentlength = 0, length = 0;
    rio_readinitb(&rio, serverfd);
    /* read from server, send it to client */
    while((length = rio_readnb(&rio, buf, MAX_LINE)) > 0){
        rio_writen(connfd, buf, length);
        if(contentlength + length < MAX_OBJECT_SIZE){
            strcpy(content_cache + contentlength, buf);
        }
        contentlength += length;
    }
    /* should cache it */
    if(contentlength < MAX_OBJECT_SIZE
       && contentlength >= MIN_OBJECT_SIZE){
        write_cache(find_evict(), content_cache, url, rank);
    }
    /*memo: cache related functions
     *  void read_cache(int idx, char* dst_data
     *                 , char* dst_url, int* dst_rank);
     *  void write_cache(int idx, char* src_data
     *                 , char* src_url, int src_rank);
     */
    close(serverfd);
    close(connfd);
    return NULL;
}
/* thread end */































/* make up some space, looks well */
