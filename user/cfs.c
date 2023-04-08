#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()
{

  int child1 = fork();
  int child2 = 0;
  if (child1 == 0) {
    //sleep(10);
    set_cfs_priority(2);
  }
  if (child1 != 0) {
    child2 = fork();
    if (child2 == 0) {
      //sleep(20);
      set_cfs_priority(0);
    }
  }
  if(child1 && child2){
    //sleep(30);
  }
  for (int i = 0; i < 1000000; i++)
  {
    if (i % 100000 == 0) {
      sleep(1);
    }    
  }
  int proc_data[4];
  int pid = getpid();
  get_cfs_stats(pid, proc_data);
  printf("PID: %d CFS priority: %d Running time: %d Runnable time: %d Sleep time: %d\n", pid, proc_data[0],
    proc_data[1], proc_data[2], proc_data[3]);

  wait(0);
  wait(0);
  return 0;
}
