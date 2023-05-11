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

void* func() {
  kthread_exit(0);
  return 0;
}
void* errorfunc() {
  printf("[ERROR] shouldn't print this function\n");
  kthread_exit(1);
  return 0;
}
void* fib() {
  int t1 = 0;
  int t2 = 1;
  int nextTerm = t1 + t2;
  for (int i = 0;i < 100000000;i++) {
    t1 = t2;
    t2 = nextTerm;
    nextTerm = t1 + t2;
  }
  kthread_exit(0);
  return 0;
}
void* infunc() {
  for (;;) {
    //fib10();
  }
  return 0;
}
/**
 * note: the variable name of the stack_size macro is MAX_STACK_SIZE. change it to the name of the macro you defined.
*/
int main(int argc, char** argv) {
  printf("This test will test the Behavior of kthread_create\n\
    THIS TEST ASSUME THAT A PROCESS CAN HAVE 10 THREADS!\n\
    TEST 1: Trying to create a thread\n\
    Expected Behvaiour:\nSuccefully creating a new thread. The new thread's id should be 2.\n\
    for further testing, one should run this test while debugging allocproc after running the command ls for example.\n\
    This is in order to check that the main thread is the first thread in the kthread array\n");
  sleep(5);
  void* memory = malloc(MAX_STACK_SIZE);
  int tid = kthread_create(func, (uint64)memory, MAX_STACK_SIZE);
  fprintf(2, "[main] created %d\n", tid);
  if (tid != 2) {
    fprintf(2, "[ERROR] got tid=%d,should have been 1\n", tid);
    return 0x66;
  }

  kthread_join(tid, 0);
  free(memory);
  memory = 0;
  /*printf("TEST 2: Trying to create a thread with NULL pointer as stack\n\
  Expected Behvaiour:\n\
  The system call kthread_create should fail and return -1.\n");*/
  // sleep(5);
  // if ((tid = kthread_create(errorfunc, (uint64)0, MAX_STACK_SIZE)) != -1) {
  //     kthread_join(tid,0);
  //     fprintf(2,"[ERROR] TEST 2 FAILED\n got tid=%d instead of -1",tid);
  //     return 0x66;
  // }
   /*printf("TEST 3: Trying to create threads with too much memory allocated for the thread's stack\n\
   Expected Behvaiour:\n\
   kthread_create should fail and return -1\n");*/
   // sleep(5);
   // memory = malloc(2*MAX_STACK_SIZE);
   // if ((tid = kthread_create(errorfunc, (uint64)memory, 2 * MAX_STACK_SIZE)) != -1) {
   //     kthread_join(tid,0);
   //     fprintf(2,"[ERROR] TEST 3 FAILED\n got tid=%d instead of -1\n",tid);
   //     free(memory);
   //     return 0x66;
   // }
   // free(memory);
   // memory=0;
  printf("TEST 4: Trying to create too much threads\n\
    Expected Behvaiour:\n\
    10 successfull kthread_create and 1 failed attempt.\n");
  void* memories[11];
  int ktid[11];
  for (int i = 0;i < 9;i++) {
    memories[i] = malloc(MAX_STACK_SIZE);
    if ((ktid[i] = kthread_create(fib, (uint64)memories[i], MAX_STACK_SIZE)) == -1) {
      fprintf(2, "[ERROR] TEST 4 FAILED\n got -1 in iteration:%d\n", i);
      return 0x66;
    }
  }
  memories[10] = malloc(MAX_STACK_SIZE / 2);
  if ((ktid[10] = kthread_create(fib, (uint64)memories[10], MAX_STACK_SIZE)) != -1) {
    fprintf(2, "[ERROR] TEST 4 FAILED\n got tid = %d instead of -1.\n", ktid[10]);
    return 0x66;
  }
  for (int i = 0;i < 11;i++) {
    if (ktid[i] != -1) {
      kthread_kill(ktid[i]);
    }
  }
  printf("Success\n");
  return 1;
}
