#include "ustack.h"
#include <stddef.h>
#include "user/user.h"


typedef struct header Header;

static Header* head;
static Header base;
static int freed;

void* ustack_malloc(uint len)
{
  printf("running malloc; len = %d\n", len);
  uint nunits;
  char* p;
  Header* hp;

  if (len > MAXSIZE) {
    return (void*)-1;
  }

  nunits = (len + sizeof(Header) - 1) / sizeof(Header) + 1;
  printf("running malloc; nunits = %d; one nunit = %d bytes\n", nunits, sizeof(Header));
  if (head == NULL) {
    base.size = 0;
    base.prev = 0;
    head = &base;
  }

  p = sbrk(nunits * sizeof(Header));
  if (p == (char*)-1)
    return (void*)-1;

  hp = (Header*)p;
  hp->prev = head;
  hp->size = nunits;
  head = hp;

  return (void*)(p + 1);
}


int ustack_free(void)
{
  char* p;
  
  // if(head->size == 0 && head->prev == 0){
  //   return -1;
  // }
  if(head == &base){
    return -1;
  }
  
  int removing = head->size * sizeof(Header);

  printf("running free; removing = %d\n", removing);

  if ((freed + removing) > PGSIZE) {
    printf("got PGSIZE\n");
    p = sbrk(-PGSIZE);
    if (p == (char*)-1)
      return -1;
    freed = freed + removing - PGSIZE;
  }
  else{
    freed += removing;
  }
  head = head->prev;
  return removing;

}
