#include "user.h"

void* func(){
    kthread_exit(0);
    return 0;
}

int main(int argc, char** argv){
    printf("This test will try to call for kthread_exit from 3 threads thread\nExpected Behvaiour: sucessfull exit and return to the shell\n");
    void* memory = malloc(4000);
    void* memory2 = malloc(4000);
    int tid = kthread_create(func,memory,4000);
    fprintf(2,"[main] created %d\n",tid);
    tid = kthread_create(func,memory2,4000);
    fprintf(2,"[main] created %d\n",tid);
    kthread_exit(0);
    return 1;
}