// #ifndef PROC_H
// #define PROC_H

#include "kthread.h"

enum procstate { UNUSED, USED, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock;


  int nexttid;                 // Next kthread id
  struct spinlock tid_lock;    // Kthread id lock

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID                          

  struct kthread kthread[NKT];        // kthread group table
  struct trapframe *base_trapframes;  // data page for trampolines

  struct proc *parent;         // Parent process

  uint64 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // User page table
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
};

// #endif