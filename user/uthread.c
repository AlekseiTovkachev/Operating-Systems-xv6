#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "uthread.h"
#include <stddef.h>

struct uthread uthread[MAX_UTHREADS] = { 0 };


/// @brief The function should initialize the user thread in a free entry in the table,
/// but not run it just yet. Once the thread’s fields are all initialized,
/// the user thread’s state is set to runnable.
/// @param start_func 
/// @param  
/// @return 
int uthread_create(void (*start_func)(), enum sched_priority priority) {

  for (int i = 0; i < MAX_UTHREADS; i++)
  {
    if (uthread[i].state == FREE) {

      uthread[i].state = RUNNABLE;
      uthread[i].context.ra = (uint64)start_func;
      memset(uthread[i].ustack, 0, STACK_SIZE);
      uthread[i].context.sp = (uint64) & (uthread[i].ustack[0]);
      uthread[i].priority = priority;

      return 0;
    }
  }

  return -1;
}

void uthread_yield() {

  struct uthread* current = uthread_self();
  struct uthread* chosen = choose_runnable_thread();

  if (chosen == NULL) {
    return;
  }

  current->state = RUNNABLE;
  chosen->state = RUNNING;
  uswtch(&(current->context), &(chosen->context));



  // for (int i = 0; i < MAX_UTHREADS; i++)
  // {
  //   if (uthread[i].state == RUNNABLE) {
  //     current->state = RUNNABLE;
  //     scheduler();
  //   }
  // }

  // return;
}


void uthread_exit() {

  struct uthread* current = uthread_self();
  struct uthread* chosen = choose_runnable_thread();

  if (chosen == NULL) {
    exit(0);
  }

  current->state = FREE;
  chosen->state = RUNNING;
  uswtch(&(current->context), &(chosen->context));
}


int uthread_start_all() {

  static int first = 1;

  if (first) {

    struct uthread* chosen_thread = choose_runnable_thread();
    // choose thread
    if (chosen_thread == NULL) {
      return -1;
    }
    first = 0;

    // context switch
    uswtch((struct context*)NULL, &(chosen_thread->context));

    // Not accessible
    return -1;
  }
  else {
    return -1;
  }

}

enum sched_priority uthread_set_priority(enum sched_priority priority) {

  struct uthread* current = uthread_self();
  int cur_p = current->priority;
  current->priority = priority;

  return cur_p;
}

enum sched_priority uthread_get_priority() {
  return uthread_self()->priority;
}

struct uthread* uthread_self() {
  for (int i = 0; i < MAX_UTHREADS; i++)
  {
    if (uthread[i].state == RUNNING)
      return &(uthread[i]);
  }

  //Not accessible
  return NULL;
}

// void scheduler() {
//   struct uthread* current = uthread_self();
//   struct uthread* chosen = choose_runnable_thread();
// }

struct uthread* choose_runnable_thread() {

  struct uthread* current = uthread_self();

  for (int i = 0; i < MAX_UTHREADS; i++)
  {
    if (!(&uthread[i] == current) && (uthread[i].state == RUNNABLE) && (uthread[i].priority == HIGH)) {
      return &uthread[i];
    }
  }
  for (int i = 0; i < MAX_UTHREADS; i++)
  {
    if (!(&uthread[i] == current) && (uthread[i].state == RUNNABLE) && (uthread[i].priority == MEDIUM)) {
      return &uthread[i];
    }
  }
  for (int i = 0; i < MAX_UTHREADS; i++)
  {
    if (!(&uthread[i] == current) && (uthread[i].state == RUNNABLE) && (uthread[i].priority == LOW)) {
      return &uthread[i];
    }
  }
  return NULL;
}
