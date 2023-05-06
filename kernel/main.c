#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator
    // printf("kinit\n");
    kvminit();       // create kernel page table
    // printf("kvminit\n");
    kvminithart();   // turn on paging
    // printf("kvminithart\n");
    procinit();      // process table
    // printf("procinit\n");
    trapinit();      // trap vectors
    // printf("trapinit\n");
    trapinithart();  // install kernel trap vector
    // printf("trapinithart\n");
    plicinit();      // set up interrupt controller
    // printf("plicinit\n");
    plicinithart();  // ask PLIC for device interrupts
    // printf("plicinit\n");
    binit();         // buffer cache
    // printf("binit\n");
    iinit();         // inode table
    // printf("iinit\n");
    fileinit();      // file table
    // printf("fileinit\n");
    virtio_disk_init(); // emulated hard disk
    // printf("virtio_disk_init\n");
    userinit();      // first user process
    // printf("userinit\n");
    __sync_synchronize();
    // printf("__sync_synchronize\n");
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }
  printf("scheduler\n");
  scheduler();        
}
