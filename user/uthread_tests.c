#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "uthread.h"

static int counter = 0;

void thread_func1() {
  while (1) {
    printf("Thread 1\n");
    if (counter++ > 20) {
      uthread_exit();
    }
    uthread_yield();
  }
}

void thread_func2() {
  while (1) {
    printf("Thread 2\n");
    if (counter++ > 20) {
      uthread_exit();
    }
    uthread_yield();
  }
}

void thread_func3() {
  while (1) {
    printf("Thread 3\n");
    if (counter++ > 20) {
      uthread_exit();
    }
    uthread_yield();
  }
}

void thread_func4() {
  while (1) {
    printf("Thread 4\n");
    if (counter++ > 20) {
      uthread_exit();
    }
    uthread_yield();
  }
}

int main() {

  uthread_create(thread_func1, MEDIUM);
  uthread_create(thread_func2, MEDIUM);
  uthread_create(thread_func3, LOW);
  uthread_create(thread_func4, HIGH);

  uthread_start_all();

  return 0;
}