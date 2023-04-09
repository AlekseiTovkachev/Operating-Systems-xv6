#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  char exit_msg[32];
  argstr(1, exit_msg, 32);
  argint(0, &n);
  exit(n, exit_msg);
  return 0;  // not reached
}


// uint64
// sys_exit(void)
// {
//   int n;
//   argint(0, &n);
//   exit(n);
//   return 0;  // not reached
// }

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

// uint64
// sys_wait(void)
// {
//   uint64 p;
//   argaddr(0, &p);
//   return wait(p);
// }

uint64
sys_wait(void)
{
  uint64 p;
  uint64 exit_msg;
  argaddr(0, &p);
  argaddr(1, &exit_msg);
  return wait(p, (char*)exit_msg);
}


uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Updates process priority (0, 1 or 2)
// Returns 0 on success and -1 on failure
uint64
sys_set_cfs_priority(void)
{
  int priority;
  argint(0, &priority);
  if(priority == 0){
    myproc()->cfs_priority = 75;
  }
  else if(priority == 1){
    myproc()->cfs_priority = 100;
  }
  else if(priority == 2){
    myproc()->cfs_priority = 125;
  }
  else{
    return -1;
  }
  return 0;
}

uint64
sys_get_cfs_stats(void)
{
  int pid;
  uint64 data;
  argint(0, &pid);
  argaddr(1, &data);
  return get_cfs_stats(pid, data);
}

uint64
sys_set_ps_priority(void)
{
  int n;
  struct proc* p = myproc();
  argint(0, &n);
  if ((n < 1) || (n > 10)) {
    return -1;
  }
  acquire(&p->lock);
  p->ps_priority = n;
  release(&p->lock);
  return 0;
}

uint64
sys_memsize(void)
{
  struct proc* p = myproc();
  return p->sz;
}

uint64
sys_set_policy(void)
{
  int policy;
  argint(0, &policy);
  return set_policy(policy);
}