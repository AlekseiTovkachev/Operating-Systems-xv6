#include "ustack.h"
#include "kernel/types.h"
// #include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
  printf("Starting tests\n");



  for (int i = 0; i < 10; i++)
  {
    char* p = (char*)ustack_malloc(512);
    p = "hello world\n";
    printf(p);
  }
  for (int i = 0; i < 10; i++)
  {
    int r;
    if ((r = ustack_free()) == -1) {
      printf("fail during free\n");
    }
    else{
      printf("removed %d bytes\n", r);
    }
  }
 


  if(ustack_free() == -1){
    printf("GOOOOOD\n");
  }
  printf("Done\n");







  
  return 0;
}
