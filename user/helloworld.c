#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()
{
  //printf("hello world\n");
  printf("Memsize: %d\n", memsize());
  return 0;
}
