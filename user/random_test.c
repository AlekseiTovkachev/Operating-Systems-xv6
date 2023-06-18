#ifndef TESTS_H
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#endif

// Linear feedback shift register
// Returns the next pseudo-random number
// The seed is updated with the returned value
uint8 lfsr_char(uint8 lfsr)
{
  uint8 bit;
  bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 0x01;
  lfsr = (lfsr >> 1) | (bit << 7);
  return lfsr;   
}


// Testing read once to compare to lfsr of original seed (0x2A)
int
test_first_with_original_seed(int fd)
{
  uint8 res;
  if (read(fd, &res, 1) < 0)
    return 0;

  if (res == lfsr_char(0x2A))
    return 1;
  return 0;
}

// Testing read 255 times to ensure correct generation of all values
// and checking that the PRG returns to the inital value after 255 applications 
int
test_looping(int fd)
{
  uint8 prev, res;
  if (read(fd, &prev, 1) < 0)
    return 0;
  uint8 firstRead = prev;

  for(int i = 0; i < 255; i++)
  {
    if (read(fd, &res, 1) < 0)
      return 0;
    if (lfsr_char(prev) != res)
      return 0;
    prev = res;
  }

  if (res != firstRead)
    return 0;

  return 1;
}

// Set the seed of the PRG to the given seed
int
set_seed(int fd, uint8 seed)
{
  return write(fd, &seed, 1);
}

// Checking whether the PRG can resume operation once the device has been close
// and reopened at a later time
int
test_continuity(int fd)
{
  uint8 prev;
  if (read(fd, &prev, 1) < 0)
    return 0;
  if (close(fd) < 0)
    return 0;
  
  if ((fd = open("random", O_RDWR)) < 0)
    return 0;
  
  uint8 res;
  if (read(fd, &res, 1) < 0)
    return 0;

  if (lfsr_char(prev) != res)
    return 0;
  
  return 1;
}

// Checks whether multiple processes generating with the same PRG
// result in the proper amount of calls generated with correct values.
int
test_concurrency_read(int fd)
{
  uint8 res, first;
  if (read(fd, &res, 1) < 0)
    return 0;
  first = res;

  if (fork() == 0)
  {
    if (read(fd, &res, 1) < 0)
      return 0;
    if (read(fd, &res, 1) < 0)
      return 0;
    if (read(fd, &res, 1) < 0)
      return 0;
    exit(0);
  }
  else
  {
    if (read(fd, &res, 1) < 0)
      return 0;
    if (read(fd, &res, 1) < 0)
      return 0;
    if (read(fd, &res, 1) < 0)
      return 0;
    if (read(fd, &res, 1) < 0)
      return 0;
  }

  wait(0);
  if (read(fd, &res, 1) < 0)
    return 0;

  if (write(fd, &first, 1) == -1)
    return 0;

  for(int i = 0; i < 8; i++)
    if (read(fd, &first, 1) < 0)
      return 0;

  if (res != first)
    return 0;

  return 1;
}

// Checks whether writing a new seed while another process generates using the same PRG
// results in the reading process generating random values using the newly written seed
int
test_concurrency_write(int fd, int print)
{
  uint8 newSeed = 0x38;
  uint8 res;
  if(fork() == 0)
  {
    if (write(fd, &newSeed, 1) == -1)
      return 0;
    exit(0);
  }
  else
  {
    for(int i = 0; i < 10; i++)
    {
      if (read(fd, &res, 1) < 0)
        return 0;
    }
  }
  wait(0);
  if (read(fd, &res, 1) < 0)
    return 0;
  uint8 prev = newSeed;
  for(int i = 0; i < 11; i++)
  {
    if (prev == res)
    {
      if (print) printf("\nWrite concurrency test: Reading on new seed occured %d times!\n\n", i);
      return 1;
    }
    prev = lfsr_char(prev);
  }

  if (prev == res)
    return 1;

  return 0;
}

// Checks writing with n != 1 returns -1
int
test_fail_write(int fd)
{
  uint8 newSeed = 0x45;
  if (write(fd, &newSeed, 6) != -1)
    return 0;
  
  return 1;
}

// Prints out the error message received, sets seed to the original seed 0x2A and closes the device
void
handleFailure(char *errorMsg, int fd)
{
  printf("%s\n", errorMsg);
  set_seed(fd, 0x2A);
  close(fd);
}

int
test_random(int print)
{
  int fd = open("random", O_RDWR | O_CREATE);
  if (fd < 0)
    {
      handleFailure("ERROR: OPENING RANDOM DEVICE", fd);
      return 0;
    }

  if (test_first_with_original_seed(fd) == 0)
    {
      handleFailure("ERROR: Failed reading original seed", fd);
      return 0;
    }
  if (print) printf("SUCCESS: Passed reading original seed\n");

  if (test_looping(fd) == 0)
  {
    handleFailure("ERROR: Failed looping 255 to return to original seed", fd);
    return 0;
  }
  if (print) printf("SUCCESS: Passed looping to regenerate original seed\n");

  if (test_fail_write(fd) == 0)
  {
    handleFailure("ERROR: Failed writing new seed with n != 1 returned 1 rather than -1", fd);
    return 0;
  }
  if (print) printf("SUCCESS: Passed calling write with n != 1 failed successfuly!\n");
  
  if (set_seed(fd, 0x4C) == -1)
    {
    handleFailure("ERROR: Failed writing a new seed", fd);
    return 0;
  }
  if (print) printf("SUCCESS: Passed writing a new seed\n");

  if (test_looping(fd) == 0)
  {
    handleFailure("ERROR: Failed looping 255 to return to new seed", fd);
    return 0;
  }
  if (print) printf("SUCCESS: Passed looping to regenerate new seed\n");

  if (test_continuity(fd) == 0)
  {
    handleFailure("ERROR: Failed to continue generating from point before closing the device", fd);
    return 0;
  }
  if (print) printf("SUCCESS: Passed continuity test to continue generating seed from point before closing the device\n");

  if (test_concurrency_read(fd) == 0)
  {
    handleFailure("ERROR: Failed reading proper values concurrently", fd);
    return 0;
  }
  if (print) printf("SUCCESS: Passed generating proper values concurrently\n");

  if (test_concurrency_write(fd, print) == 0)
  {
    handleFailure("ERROR: Failed writing new seed concurrently", fd);
    return 0;
  }
  if (print) printf("SUCCESS: Passed writing new seed concurrently\n");

  if (set_seed(fd, 0x2A) == -1)
  {
    handleFailure("ERROR: Failed resetting to original seed", fd);
    return 0;
  }
  close(fd);
  printf("SUCCESS! Passed all RANDOM tests!\n");
  return 1;
}
