#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];

// int nexttid = 1;
// struct spinlock tid_lock;

void kthreadinit(struct proc* p)
{

  initlock(&tid_lock, "nexttid");
  for (struct kthread* kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    initlock(&kt->lock, "kthread");

    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
    kt->state = UNUSED;
    kt->parent = p;
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
int
cpuid()
{
  int id = r_tp();
  return id;
}

struct cpu*
  mycpu(void)
{
  int id = cpuid();
  struct cpu* c = &cpus[id];
  return c;
}

struct kthread* mykthread()
{
  push_off();
  struct cpu* c = mycpu();
  struct proc* p = c->proc;
  struct kthread* kt;
  for (int i = 0; i < NKT; i++)
  {
    if (p->ktherad[i].state == RUNNING) {
      kt = &p->kthread[i];
      break;
    }
  }
  pop_off();
  return kt;
  //return &myproc()->kthread[0];
}

int
alloctid()
{
  int tid;

  acquire(&tid_lock);
  tid = nexttid;
  nexttid = nexttid + 1;
  release(&tid_lock);

  return tid;
}

static struct kthread*
allockthread(void)
{
  struct kthread* kt;
  struct proc* p = mycpu()->proc;
  for (int i = 0; i < NKT; i++)
  {
    kt = &p->kthread[i];
    acquire(&kt->lock);
    if (kt->state == UNUSED) {
      goto found;
    }
    else {
      release(&kt->lock);
    }
  }
  return 0;


  // for (kt = p->kthread[0]; kt < &p->kthread[NKT]; kt++) {
  //   acquire(&kt->lock);
  //   if (kt->state == UNUSED) {
  //     goto found;
  //   }
  //   else {
  //     release(&kt->lock);
  //   }
  // }
  // return 0;

found:
  kt->tid = alloctid();
  kt->state = USED;
  kt->trapframe = get_kthread_trapframe(p, kt);

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&kt->context, 0, sizeof(kt->context));
  kt->context.ra = (uint64)forkret;
  kt->context.sp = kt->kstack + PGSIZE;

  return kt;
}


static void
freekthread(struct kthread* kt)
{
  kt->trapframe = 0;

  kt->tid = 0;
  kt->parent = 0;
  kt->chan = 0;
  kt->killed = 0;
  kt->xstate = 0;
  kt->state = UNUSED;

}

struct trapframe* get_kthread_trapframe(struct proc* p, struct kthread* kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}

// TODO: delete this after you are done with task 2.2
void allocproc_help_function(struct proc* p) {
  p->kthread->trapframe = get_kthread_trapframe(p, p->kthread);

  p->context.sp = p->kthread->kstack + PGSIZE;
}
