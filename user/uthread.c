#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "uthread.h"

struct uthread uthread[MAX_UTHREADS];


/// @brief The function should initialize the user thread in a free entry in the table,
/// but not run it just yet. Once the thread’s fields are all initialized,
/// the user thread’s state is set to runnable.
/// @param start_func 
/// @param  
/// @return 
int uthread_create(void (*start_func)(), enum sched_priority priority) {

  if (uthread[MAX_UTHREADS - 1] != null) {
    //no more space for new threads
    return -1;
  }

  struct uthread new_thread = malloc(sizeof(struct uthread));
  new_thread.state = RUNNABLE;
  new_thread.context.ra = start_func;
  new_thread.context.sp = new_thread.ustack[STACK_SIZE - 1];
  new_thread.priority = priority;

  return 0;
}

void uthread_yield() {
  //TODO: update for scheduling policy

  //Round-robin
  struct uthread next = null;
  struct uthread current = &(uthread_self());

  int num_of_threads = 0;
  int index_of_current = 0;
  for (int j = 0; j < MAX_UTHREADS; j++)
  {
    if ((uthread[j] != null) && (uthread[j].state != FREE)) {
      num_of_threads++;
      if (uthread[j] == current)
        index_of_current = j;
    }
  }

  if (num_of_threads == 1)
    return;

  int i = index_of_current + 1;
  while (next == null) {

    if (i == MAX_UTHREADS)
      i = 0;

    if ((uthread[i] != null) && (uthread[j].state != FREE)) {
      next = uthread[i];
      uswtch(&(current.context), &(next.context));
      return;
    }
    i++;

  }
}


void uthread_exit() {
  //TODO: update for scheduling policy

  //Round-robin
  struct uthread next = null;
  struct uthread current = &(uthread_self());

  int num_of_threads = 0;
  int index_of_current = 0;
  for (int j = 0; j < MAX_UTHREADS; j++)
  {
    if ((uthread[j] != null) && (uthread[j].state != FREE)) {
      num_of_threads++;
      if (uthread[j] == current)
        index_of_current = j;
    }
  }

  if (num_of_threads == 1)
    exit();

  int i = index_of_current + 1;
  while (next == null) {

    if (i == MAX_UTHREADS)
      i = 0;

    if ((uthread[i] != null) && (uthread[j].state != FREE)) {
      next = uthread[i];
      uswtch(&(current.context), &(next.context));
      current.state = FREE;
      return;
    }
    i++;
  }

}


int uthread_start_all() {
  static int first = 1;
  if (first) {
    
    //choose thread
    first = 0;
    //call thread func

  }

  //if in main make switch
  //if not in main return -1
}


enum sched_priority uthread_set_priority(enum sched_priority priority) {

  struct uthread* current = uthread_self();
  sched_priority cur_p = current->priority;
  current->priority = priority;

  return cur_p;
}

enum sched_priority uthread_get_priority() {
  return uthread_self()->priority;
}

struct uthread* uthread_self() {
  for (int i = 0; i < MAX_UTHREADS; int i++)
  {
    if ((uthread[i] != null) && (uthread[i].state == RUNNING))
      return &(uthread[i]);
  }

  //Not accessible
  return null;
}
