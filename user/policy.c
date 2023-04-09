#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



int main(int argc, char** argv) {
  if(argv[1][1] != '\0'){
    printf("Failure :(\n");
    return 0;
  }
  int policy = argv[1][0] - '0';

  if (set_policy(policy) == 0)
    printf("Success!!!\n");
  else
    printf("Failure :(\n");
  return 0;
}