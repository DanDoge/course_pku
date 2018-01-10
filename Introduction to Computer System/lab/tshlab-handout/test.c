#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

sigjmp_buf buf;

void handler(int sig){
    siglongjmp(buf, 1);
}

int main(){
    printf("main enter\n");
    if(signal(SIGSEGV, handler) == SIG_ERR){
        printf("signal error\n");
    }
    unsigned long tmp = 0;
    unsigned long i = 0;
    long cnt = 0;
    while(cnt != ((long)1 << 52) - 1){
        sigsetjmp(buf, 1);
        i += 1 << 12;
        cnt += 1;
        tmp = *(unsigned long* )i;
        printf("pagenum: %lx\n", i);
    }
    return 0;
}
