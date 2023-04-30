#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "uthread.h"

void thread_func1() {
  while (1) {
    printf("Thread 1\n");
    exit(0);
    // uthread_yield();
  }
}

void thread_func2() {
  while (1) {
    printf("Thread 2\n");
    uthread_yield();
  }
}

void thread_func3() {
  while (1) {
    printf("Thread 3\n");
    uthread_yield();
  }
}

void thread_func4() {
  while (1) {
    printf("Thread 4\n");
    uthread_yield();
  }
}

int main() {

  // printf("hw");
  uthread_create(thread_func1, MEDIUM);
  // uthread_create(thread_func2, MEDIUM);
  // uthread_create(thread_func3, LOW);
  // uthread_create(thread_func4, HIGH);

  uthread_start_all();

  return 0;
}