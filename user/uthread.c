#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "uthread.h"
#include <stddef.h>

struct uthread uthread[MAX_UTHREADS];

static int started = 0;
static struct uthread* current;



int uthread_create(void (*start_func)(), enum sched_priority priority) {

  for (int i = 0; i < MAX_UTHREADS; i++)
  {
    if (uthread[i].state == FREE) {

      uthread[i].state = RUNNABLE;
      uthread[i].priority = priority;

      uthread[i].context.ra = (uint64)start_func;
      uthread[i].context.sp = (uint64) & (uthread[i].ustack[STACK_SIZE - 1]);

      return 0;
    }
  }

  return -1;
}

void uthread_yield() {

  struct uthread* old_t = current;
  struct uthread* new_t = choose_runnable_thread();

  if (new_t == NULL) {
    return;
  }
  old_t->state = RUNNABLE;
  new_t->state = RUNNING;
  current = new_t;

  uswtch(&old_t->context, &new_t->context);
}


void uthread_exit() {

  struct uthread* old_t = current;
  struct uthread* new_t = choose_runnable_thread();

  if (new_t == NULL) {
    exit(0);
  }

  old_t->state = FREE;
  new_t->state = RUNNING;
  current = new_t;

  uswtch(&old_t->context, &new_t->context);
}


int uthread_start_all() {

  if ((started > 0) || (current != NULL))
    return -1;

  current = choose_runnable_thread();

  struct context dummy;
  memset(&dummy, 0, sizeof(struct context));

  uswtch(&dummy, &current->context);

  // Not accessible
  return -1;
}



enum sched_priority uthread_set_priority(enum sched_priority priority) {

  enum sched_priority cur_p = current->priority;
  current->priority = priority;

  return cur_p;
}

enum sched_priority uthread_get_priority() {
  return current->priority;
}

struct uthread* uthread_self() {
  return current;
}


struct uthread* choose_runnable_thread() {

  for (int i = 0; i < MAX_UTHREADS; i++)
  {
    if ((&uthread[i] != current) && (uthread[i].state == RUNNABLE) && (uthread[i].priority == HIGH)) {
      return &uthread[i];
    }
  }
  for (int i = 0; i < MAX_UTHREADS; i++)
  {
    if ((&uthread[i] != current) && (uthread[i].state == RUNNABLE) && (uthread[i].priority == MEDIUM)) {
      return &uthread[i];
    }
  }
  for (int i = 0; i < MAX_UTHREADS; i++)
  {
    if ((&uthread[i] != current) && (uthread[i].state == RUNNABLE) && (uthread[i].priority == LOW)) {
      return &uthread[i];
    }
  }
  return NULL;
}
