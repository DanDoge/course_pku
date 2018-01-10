/*
 * Name      : 
 * Nickname  :
 * UserID    :
 * reference : "http://www.cs.cmu.edu/afs/cs/academic/class/15213-s17
 *              /www/recitations/recitation07-cachelab.pdf"
 */

#include "cachelab.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

/* global varibles, all initialized to zero */
int s = 0, E = 0, b = 0;
char trace_file_name[128] = {};
int verbose_mode = 0;
int hit_count = 0, miss_count = 0, eviction_count = 0;

/* structure for simulating cache*/
typedef struct{
    int valid;
    long tag;
    unsigned long last_used_time;
}line;

typedef struct{
    line* ptr_lines;
}set;

set* cache = NULL;

/* function declaration */
int  get_opt(int argc, char* argv[]);         // done
void print_help();                            // done
int  initcache(set* cache);                   // done
void dataload(int set_bit, int tag_bit);      // done
void datastore(int set_bit, int tag_bit);     // done
int  find_in_set(int set_bit, int tag_bit);   // done
int  is_full(int set_bit);                    // done
void replace_in_set(int set_bit, int tag_bit);// done
void update_set(int set_bit);                 // done
void free_cache(set* cache);                  // done

int main(int argc, char** argv){
    if(get_opt(argc, argv)){
        cache = (set* )malloc((1 << s) * sizeof(set));
        if(initcache(cache)){
            FILE* trace_file = fopen(trace_file_name, "r");
            char operation[16] = {};
            unsigned long address = 0;
            int size = 0;
            /* read trace, line by line */
            while(fscanf(trace_file, "%s %lx,%d", operation, &address, &size) != EOF){
                /* ignore instructions */
                if(strcmp(operation, "I") == 0){
                    continue;
                }
                if(verbose_mode){
                    printf("%s %lx,%d ", operation, address, size);
                }
                int set_bit = (address >> b) & ((1 << s) - 1);
                int tag_bit = (address >> (s + b));
                if(strcmp(operation, "M") == 0){
                    dataload(set_bit, tag_bit);
                    datastore(set_bit, tag_bit);
                }else if(strcmp(operation, "L") == 0){
                    dataload(set_bit, tag_bit);
                }else if(strcmp(operation, "S") == 0){
                    datastore(set_bit, tag_bit);
                }
                if(verbose_mode){
                    printf("\n");
                }
            }/* end of while */

            free_cache(cache);
            printSummary(hit_count, miss_count, eviction_count);
        }/* end of initcache */
    }/* end of get_opt */
    return 0;
}

/* function definition */

/* last_used_time represents time from last used till now */
void update_set(int set_bit){
    for(int i = 0; i < E; i += 1){
        cache[set_bit].ptr_lines[i].last_used_time += 1;// add one each time
    }
}

void replace_in_set(int set_bit, int tag_bit){
    int max_last_use_time = 0;
    int max_last_use_idx = E;
    /* find the least recent used line */
    for(int i = 0; i < E; i += 1){
        if(cache[set_bit].ptr_lines[i].valid == 1
           && cache[set_bit].ptr_lines[i].last_used_time > max_last_use_time){
            max_last_use_time = cache[set_bit].ptr_lines[i].last_used_time;
            max_last_use_idx = i;
        }
    }
    /* and replace it with a new one */
    cache[set_bit].ptr_lines[max_last_use_idx].tag = tag_bit;
    cache[set_bit].ptr_lines[max_last_use_idx].last_used_time = 0;
}

/* return availble line number if not full, otherwise return E */
int is_full(int set_bit){
    for(int i = 0; i < E; i += 1){
        if(cache[set_bit].ptr_lines[i].valid == 0){
            return i;
        }
    }
    return E;
}

/* literally "find in set" */
int find_in_set(int set_bit, int tag_bit){
    for(int i = 0; i < E; i += 1){
        if(cache[set_bit].ptr_lines[i].valid == 1
           && cache[set_bit].ptr_lines[i].tag == tag_bit){
            /* hit! and set its last_used_time to 0 */
            cache[set_bit].ptr_lines[i].last_used_time = 0;
            return 1;
        }
    }
    return 0;
}

/* if not in the given set, put it in */
void dataload(int set_bit, int tag_bit){
    if(!find_in_set(set_bit, tag_bit)){
        miss_count += 1;
        if(verbose_mode){
            printf("miss ");
        }
        int availble_place = 0;
        if((availble_place = is_full(set_bit)) != E){
            /* initialize this line */
            cache[set_bit].ptr_lines[availble_place].valid = 1;
            cache[set_bit].ptr_lines[availble_place].tag = tag_bit;
            cache[set_bit].ptr_lines[availble_place].last_used_time = 0;
        }else{
            replace_in_set(set_bit, tag_bit);
            eviction_count += 1;
            if(verbose_mode){
                printf("eviction ");
            }
        }
        update_set(set_bit);
    }else{
        hit_count += 1;
        update_set(set_bit);
        if(verbose_mode){
            printf("hit ");
        }
    }
}

/* same as dataload */
void datastore(int set_bit, int tag_bit){
    dataload(set_bit, tag_bit);
}

/* using the form given in ./test-trans.c */
int get_opt(int argc, char** argv){
    int tmp = -1;
    while((tmp = getopt(argc, argv, "hvs:E:b:t:")) != -1){
        switch(tmp){
            case 'h':{
                print_help();
                return 0;
            }
            case 'v':{
                verbose_mode = 1;
                break;
            }
            case 's':{
                s = atoi(optarg);
                break;
            }
            case 'E':{
                E = atoi(optarg);
                break;
            }
            case 'b':{
                b = atoi(optarg);
                break;
            }
            case 't':{
               strcpy(trace_file_name, optarg);
               break;
            }
            default:{
                print_help();
                return 0;
            }
        }
    }
    return 1;
}

void print_help(){
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("-h         Print this help message.\n");
    printf("-v         Optional verbose flag.\n");
    printf("-s <num>   Number of set index bits.\n");
    printf("-E <num>   Number of lines per set.\n");
    printf("-b <num>   Number of block offset bits.\n");
    printf("-t <file>  Trace file.\n\n\n");
    printf("Examples:\n");
    printf("linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

/* initialize cache */
int initcache(set* cache){
    int set_sum = 1 << s;
    for(int i = 0; i < set_sum; i += 1){
        cache[i].ptr_lines = (line* )malloc(E * sizeof(line));
        for(int j = 0; j < E; j += 1){
            cache[i].ptr_lines[j].valid = 0;
            cache[i].ptr_lines[j].last_used_time = 0;
        }
    }
    return 1;
}

/* and free it */
void free_cache(set* cache){
    for(int i = 0; i < 1 << s; i += 1){
        free(cache[i].ptr_lines);
    }
    free(cache);
}

/* end */
