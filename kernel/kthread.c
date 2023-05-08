#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];
extern void forkret(void);
extern struct spinlock wait_lock;
// struct cpu cpus[NCPU];


void kthreadinit(struct proc* p)
{

  initlock(&p->tid_lock, "nexttid");
  for (struct kthread* kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    initlock(&kt->lock, "kthread");

    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
    kt->state = K_UNUSED;
    kt->parent = p;
  }
}

// // Must be called with interrupts disabled,
// // to prevent race with process being moved
// // to a different CPU.
// int
// cpuid()
// {
//   int id = r_tp();
//   return id;
// }

// // Return this CPU's cpu struct.
// // Interrupts must be disabled.
// struct cpu*
//   mycpu(void)
// {
//   int id = cpuid();
//   struct cpu* c = &cpus[id];
//   return c;
// }

// Return the current struct kthehread *, or zero if none.
struct kthread*
  mykthread(void)
{
  push_off();
  struct cpu* c = mycpu();
  struct kthread* kt = c->kthread;
  pop_off();
  return kt;
}


int
alloctid(struct proc* p)
{
  int tid;

  acquire(&p->tid_lock);
  tid = p->nexttid;
  p->nexttid = p->nexttid + 1;
  release(&p->tid_lock);

  return tid;
}

struct kthread*
  allockthread(struct proc* p)
{
  struct kthread* kt;
  for (kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    acquire(&kt->lock);
    if (kt->state == K_UNUSED) {
      goto found;
    }
    else {
      release(&kt->lock);
    }
  }
  return 0;

found:
  kt->tid = alloctid(p);
  kt->state = K_USED;
  kt->trapframe = get_kthread_trapframe(p, kt);

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&kt->context, 0, sizeof(kt->context));
  kt->context.ra = (uint64)forkret;
  kt->context.sp = kt->kstack + PGSIZE;

  return kt;
}


void
freekthread(struct kthread* kt)
{
  kt->trapframe = 0;

  kt->tid = 0;
  // kt->parent = 0;
  kt->chan = 0;
  kt->killed = 0;
  kt->xstate = 0;
  kt->state = K_UNUSED;

}

struct trapframe* get_kthread_trapframe(struct proc* p, struct kthread* kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}

int
kthread_create(void* (*start_func)(), void* stack, uint stack_size)
{
  int pid;
  struct kthread* nkt;
  struct proc* p = myproc();
  struct kthread* kt = mykthread();

  // Allocate kthread.
  if ((nkt = allockthread(p)) == 0) {
    return -1;
  }

  nkt->kstack = (uint64)stack;

  // if((nkt->kstack = (uint64)malloc(KLT_STACKSIZE)) < 0){
  //   release(nkt->lock);
  //   freekthread(nkt);
  //   return -1;
  // }

  nkt->trapframe->epc = start_func;
  nkt->trapframe->sp = (uint64)(stack + stack_size - 1);
  nkt->state = K_RUNNABLE;

  // release(nkt->lock);
}

int
kthread_id()
{
  return mykthread()->tid;
}

int
kthread_kill(int tid)
{

  struct proc* p = myproc();
  for (struct kthread* kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    acquire(&kt->lock);
    if (kt->tid == tid) {
      kt->killed = 1;
      if (kt->state == K_SLEEPING) {
        kt->state = K_RUNNABLE;
      }
      release(&kt->lock);
      return 0;
    }
    release(&kt->lock);
  }

  return -1;
}

void
kthread_setkilled(struct kthread* kt)
{
  acquire(&kt->lock);
  kt->killed = 1;
  release(&kt->lock);
}

int
kthread_killed(struct kthread* kt)
{
  int k;

  acquire(&kt->lock);
  k = kt->killed;
  release(&kt->lock);
  return k;

}

void
kthread_exit(int status)
{
  struct kthread* kt = mykthread();
  struct proc* p = myproc();

  acquire(&kt->lock);
  kt->xstate = status;
  kt->state = K_ZOMBIE;
  // Check this!
  release(&kt->lock);
  int found = 0
    for (struct kthread* okt = p->kthread; okt < &p->kthread[NKT] & !found; okt++)
    {
      acquire(&okt->lock);
      if (okt->state > K_UNUSED && okt->state < K_ZOMBIE)
        found = 1;
      release(&okt->lock);
    }

  if (!found)
    exit(status);

}

int
kthread_join(int ktid, int* status)
{
  struct kthread* kt = mykthread();
  struct kthread* kkt;
  struct proc* p = myproc();
  int found = 0;
  
  for (kkt = p->kthread; kkt < &p->kthread[NKT]; kkt++)
  {
    acquire(&kkt->lock);
    if (kkt->tid == ktid) {
      found = 1;
      break;
    }
    release(&kkt->lock);
  }
  release(&kkt->lock);

  if (found == 0) {    
    return -1;
  }
  
  acquire(&wait_lock);

  for (;;) {
    acquire(&kkt->lock);
    if (kkt->state == K_ZOMBIE) {
      if ((uint64)status != 0 && copyout(p->pagetable,
        (uint64)status, (char*)&kkt->xstate, sizeof(kkt->xstate)) < 0)
      {
        release(&kkt->lock);
        release(&wait_lock);
        return -1;
      }
      freekthread(kkt);

      release(&kkt->lock);
      release(&wait_lock);

      return 0;
    }
    release(&kkt->lock);


    
    sleep(kkt, &wait_lock);
  }

}










