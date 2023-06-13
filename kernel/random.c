#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

uint8 lfsr;
static struct spinlock lock;
uint8 lfsr_char();


int
randomwrite(int user_src, uint64 src, int n)
{
  if(n == 1){
    uint8 byte;
    copyin(myproc()->pagetable, (char*)&byte, src, 1);
    acquire(&lock);
    lfsr = byte;
    release(&lock);
    return 1;
  }
  return -1;
}

int
randomread(int user_dst, uint64 dst, int n)
{
  char buf[n];
  for (int i = 0; i < n; i++)
  {
    buf[i] = lfsr_char();
  }
  if (copyout(myproc()->pagetable, dst, (char*)&buf, n) < 0)
    return -1;
  
  return n;
}

void
randominit(void)
{
  initlock(&lock, "random");
  lfsr = 0x2A;
  // uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[RANDOM].read = randomread;
  devsw[RANDOM].write = randomwrite;
}


// Linear feedback shift register
// Returns the next pseudo-random number
// The seed is updated with the returned value
uint8 lfsr_char()
{
  acquire(&lock);
  uint8 bit;
  bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 0x01;
  lfsr = (lfsr >> 1) | (bit << 7);
  release(&lock);
  return lfsr;
}