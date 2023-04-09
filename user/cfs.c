#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char waitbuf[32];

int
main()
{

  int main_pid = getpid();
  int child_pids[3];
  for (int i = 0; i < 3; i++)
  {
    if(getpid() == main_pid)
      child_pids[i] = fork();
  }
  if (child_pids[0] == 0) {
    set_cfs_priority(0);
    sleep(10);
  }
  if ((child_pids[0] != 0) && (child_pids[1] == 0)) {
    set_cfs_priority(1);
    sleep(20);
  }
  if ((child_pids[0] != 0) && (child_pids[1] != 0)) {
    set_cfs_priority(2);
    sleep(30);
  }

  if(getpid() == main_pid){
    wait(0, waitbuf);
    wait(0, waitbuf);
    wait(0, waitbuf);
    return 0;
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

  return 0;
}
