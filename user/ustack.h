#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/riscv.h"

#define MAXSIZE 512 // maximum size for allocation

typedef struct header{
  uint size;
  uint dealloc_page;
  struct header* prev;
} header;


void* ustack_malloc(uint len);
int ustack_free(void);

