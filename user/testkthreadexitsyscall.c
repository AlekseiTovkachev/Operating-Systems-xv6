#include "user.h"

void* func(){
    sleep(1);
    exit(0);
    return 0;
}

int main(int argc, char** argv){
    printf("This test will try to call for exit from 2 threads thread\nExpected Behvaiour: sucessfull exit and return to the shell\n");
    void* memory = malloc(4000);
    kthread_create(func,memory,4000);
    exit(1);
    return 1;
}