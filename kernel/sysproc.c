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
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

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

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
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
  while (ticks - ticks0 < n) {
   
    if (killed(myproc())/* || kthread_killed(mykthread())*/) {
      release(&tickslock);
      return -1;
    }
    if(kthread_killed(mykthread())){
      kthread_exit(-1);
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

uint64
sys_kthread_create(void)
{
  uint64 start_func;
  uint64 stack;
  int stack_size;

  argaddr(0, &start_func);
  argaddr(1, &stack);
  argint(2, &stack_size);

  return kthread_create((void*)start_func, (void*)stack, stack_size);
}

uint64
sys_kthread_id(void)
{
  return kthread_id();
}

uint64
sys_kthread_kill(void)
{
  int tid;

  argint(0, &tid);
  return kthread_kill(tid);
}

uint64
sys_kthread_exit(void)
{
  int n;
  argint(0, &n);
  kthread_exit(n);
  return 0;  // not reached
}

uint64
sys_kthread_join(void)
{
  int ktid;
  uint64 p;
  argint(0, &ktid);
  argaddr(1, &p);
  return kthread_join(ktid, (int*)p);
}
