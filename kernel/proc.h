#include "kthread.h"

// Saved registers for kernel context switches.
// struct context {
//   uint64 ra;
//   uint64 sp;

//   // callee-saved
//   uint64 s0;
//   uint64 s1;
//   uint64 s2;
//   uint64 s3;
//   uint64 s4;
//   uint64 s5;
//   uint64 s6;
//   uint64 s7;
//   uint64 s8;
//   uint64 s9;
//   uint64 s10;
//   uint64 s11;
// };

// Per-CPU state.
// struct cpu {
//   struct proc *proc;          // The process running on this cpu, or null.
//   struct context context;     // swtch() here to enter scheduler().
//   int noff;                   // Depth of push_off() nesting.
//   int intena;                 // Were interrupts enabled before push_off()?
// };

// extern struct cpu cpus[NCPU];

//enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
enum procstate { UNUSED, USED, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock;                                                    // v

  // p->lock must be held when using these:
  enum procstate state;        // Process state                               v
  // void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed               v
  int xstate;                  // Exit status to be returned to parent's wait v
  int pid;                     // Process ID                          v

  struct kthread kthread[NKT];        // kthread group table          v
  struct trapframe *base_trapframes;  // data page for trampolines    v

  // wait_lock must be held when using this:
  struct proc *parent;         // Parent process                      v

  // these are private to the process, so p->lock need not be held.
  // uint64 kstack;               // Virtual address of kernel stack
  uint64 sz;                   // Size of process memory (bytes)      v
  pagetable_t pagetable;       // User page table                     v
  // struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files                          v
  struct inode *cwd;           // Current directory                   v
  char name[16];               // Process name (debugging)            v
};
