#ifndef TESTS_H
#define TESTS_H 
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#endif

#include "random_test.c"
#include "seek_test.c"

int main(int argc, char** argv)
{
    // Reset flag = reset PRG to original seed 0x2A
    // print flag = print non-errors during sub-tests
    int print = 0;
    int random = 0, seek = 0;
    for(int i = 0; i < argc; i++)
    {
        if (strcmp("-p", argv[i]) == 0)
            print = 1;
        if (strcmp("-seek", argv[i]) == 0)
        {
            seek = 1;
        }
        if (strcmp("-random", argv[i]) == 0)
        {
            random = 1;
        }
    }
    if (seek == 1)
    {
        if (!random)
        {
            printf("SEEK TESTS:\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            test_seek(print);
            return 0;
        }
    }
    else
    {
        if (random)
        {
            printf("RANDOM TESTS:\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            test_random(print);
            return 0;
        }
    }

    int success = 0;
    printf("SEEK TESTS:\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    success += test_seek(print);
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("\nPRNG TESTS:\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    success += test_random(print);
    printf("\n\n");
    if (success == 2)
        printf("SUCCESS!! PASSED ALL TESTS!\n");
    else
        printf("FAILED 1 OR MORE TESTS\n");
    return 0;
}