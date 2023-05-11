#include "user.h"

void* fib1000000000(){
    int t1=0;
    int t2=1;
    int nextTerm=t1+t2;
    for(int i = 0;i<1000000000;i++){
        t1=t2;
        t2=nextTerm;
        nextTerm=t1+t2;
    }
    //printf("fib 1000000000 = %d\n",nextTerm);
    kthread_exit(1);
    return 0;
}
void* fib100(){
    int t1=0;
    int t2=1;
    int nextTerm=t1+t2;
    for(int i = 0;i<100;i++){
        t1=t2;
        t2=nextTerm;
        nextTerm=t1+t2;
    }
    printf("fib 100 = %d\n",nextTerm);
    kthread_exit(1);
    return 0;
}
void* forktest(){
    char* command = "echo";
    char *argv[] = {"echo","testing fork from a multithreaded process",0};
    exec(command,argv);
    fprintf(2,"[ERROR]: shouldn't returned\n");
    return 0;
}

int main(int argc, char** argv){
    printf("This test will try to kill and join a thread\nExpected Behaviour:\nMain thread creates a thread which try to calculate fib 1000000000, then it will kill it as well as join it.\n the expected status should be 1.\nPossible Errors: Main thread doesn't return from join.\n");
    void* memory = malloc(4000);
    void* memory2 = malloc(4000);
    int returned_status;
    int tid = kthread_create(fib1000000000,memory,4000);
    kthread_kill(tid);
    if(kthread_join(tid,&returned_status) != 0){
        fprintf(2,"[ERROR] kthread_join\n");
        return -1;
    }
    int tid2 = kthread_create(fib100,memory2,4000);
    fprintf(2,"[main] created %d\n",tid2);
    kthread_join(tid2,&returned_status);
    if(returned_status != 1){
        fprintf(2,"[ERROR] wrong status\n");
        return -1;   
    }
    printf("SUCCESS!\n");
    return 1;
}