#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"


#define PAGE_SIZE 4096
#define NUM_PAGES 32
#define MAX_PHYSICAL_PAGES 16

void test() {
  // Allocate 32 pages for the process
  char* pages[NUM_PAGES];
  for (int i = 0; i < NUM_PAGES; i++) {
    pages[i] = malloc(PAGE_SIZE);
    // Access the pages to trigger page faults
    pages[i][0] = 'a';
  }
  // int pid = getpid();
  // printf("pid = %d\n", pid);
  // malloc(256);
  for (int i = 0; i < NUM_PAGES; i++) {
    printf("page number: %d, text: %s\n", i, pages[i]);
  }



  // // Free the allocated pages
  // for (int i = 0; i < NUM_PAGES; i++) {
  //   free(pages[i]);
  // }
}

int main()
{
  test();
  return 0;
}
