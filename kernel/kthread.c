#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];
struct cpu cpus[NCPU];

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

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
int
cpuid()
{
  int id = r_tp();
  return id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
struct cpu*
  mycpu(void)
{
  int id = cpuid();
  struct cpu* c = &cpus[id];
  return c;
}

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
  kt->parent = 0;
  kt->chan = 0;
  kt->killed = 0;
  kt->xstate = 0;
  kt->state = K_UNUSED;

}

struct trapframe* get_kthread_trapframe(struct proc* p, struct kthread* kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}

// // TODO: delete this after you are done with task 2.2
// void allocproc_help_function(struct proc* p) {
//   p->kthread->trapframe = get_kthread_trapframe(p, p->kthread);

//   p->context.sp = p->kthread->kstack + PGSIZE;
// }
