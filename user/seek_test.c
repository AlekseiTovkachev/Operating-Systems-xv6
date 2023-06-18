#ifndef TESTS_H
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#endif

int
test_write(int fd, int offset, int whence, char* toWrite, int length)
{
  if (seek(fd, offset, whence) < 0)
    return 0;
  if (write(fd, toWrite, length) < 0)
    return 0;
  return 1;
}

int
test_read(int fd, int offset, int whence, char* expected, int length)
{
  if (seek(fd, offset, whence) < 0)
    return 0;
  char buf[length];
  if (read(fd, buf, length) < 0)
    return 0;
  if (memcmp(expected, buf, length) == 0)
    return 1;
  return 0;
}

void
handleFailureSeek(char* errorMsg, int fd)
{
  printf("%s\n", errorMsg);
  close(fd);
}

int
openTestFile(void)
{
  int fd = open("test.txt", O_RDWR | O_CREATE | O_TRUNC);
  if (fd < 0) {
    printf("error: open test file on seek test\n");
    return 0;
  }
  return fd;
}

void
print_file_contents(int fd, int length, char* msg)
{
  char buf[length];
  seek(fd, 0, SEEK_SET);
  read(fd, buf, length);
  printf("%s\n", msg);
  for(int i = 0; i < length; i++)
    printf("%c", buf[i]);
  printf("\n");
}

// Writes data to the beginning of the file and reads it to make sure it was written properly
int
test_write_1(int fd)
{
  char toWrite[] = "Hello, World!\n";
  char expected[] = "Hello, World!\n";
  if (test_write(fd, 0, SEEK_SET, toWrite, sizeof(toWrite)) == 0)
    return 0;
  
  if (test_read(fd, 0, SEEK_SET, expected, sizeof(expected)) == 0)
    return -1;
  
  return 1;
}

// Writes data to the end of the file, note that this test is using SEEK_CUR and checks 
// too large and negative offsets
int
test_write_2(int fd)
{
  char toWrite[] = "This is new data!\n";
  char expected[] = "Hello, World!\n\0This is new data!\n";
  if (test_write(fd, 50, SEEK_CUR, toWrite, sizeof(toWrite)) == 0)
    return 0;
  
  if (test_read(fd, -50, SEEK_CUR, expected, sizeof(expected)) == 0)
    return -1;
  
  return 1;
}

// Goes back 5 chars and reads till the end of the file to make sure negative offset works with SEEK_CUR
int
test_read_1(int fd)
{
  char expected[] = "ta!\n";
  return test_read(fd, -5, SEEK_CUR, expected, sizeof(expected));
}

// Writes data to the end of the file, this test uses SEEK_SET to check too large and negative offsets
int
test_write_3(int fd)
{
  char toWrite[] = "Extra fresh data!\n";
  char expected[] = "Hello, World!\n\0This is new data!\n\0Extra fresh data!\n";
  if (test_write(fd, 500, SEEK_SET, toWrite, sizeof(toWrite)) == 0)
    return 0;
  
  if (test_read(fd, -500, SEEK_SET, expected, sizeof(expected)) == 0)
    return -1;
  
  return 1;
}

int 
test_seek(int print) {
  char expected1[] = "Hello, World!\n";
  char expected2[] = "Hello, World!\n\0This is new data!\n";
  char expected3[] = "Hello, World!\n\0This is new data!\n\0Extra fresh data!\n";

  int fd = openTestFile();
  if (fd == 0)
  {
    printf("ERROR: Error on opening txt file\n");
    return 0;
  }

  // First test: write "Hello, World!\n" to the beginning of the file, then read to make sure it was received successfully
  int res1;
  if ((res1 = test_write_1(fd)) == 1)
  {
    if (print) printf("SUCCESS! Successfully written \"Hello, World!\\n\" to the file\n");
  }
  else
  {
    if (res1 == 0)
      handleFailureSeek("ERROR: Error in writing to the beginning of the file (test_write_1)", fd);
    else
      handleFailureSeek("ERROR: Error in reading from the beginning of the file (test_write_1)", fd);
    return 0;
  }

  if (print)
    print_file_contents(fd, sizeof(expected1), "Content of the txt file after the first write:");

  // Second test: write "This is new data!\n" to the end of the file, then read to make sure it was received successfully
  // This test checks whether going too far forward (f->off + offset > file size) or too far back (f->off + offset < 0)
  // With WHENCE = SEEK_CUR
  int res2;
  if ((res2 = test_write_2(fd)) == 1)
  {
    if (print) printf("SUCCESS! Successfully written \"This is new data!\\n\" to the file\n");
  }
  else
  {
    if (res2 == 0)
      handleFailureSeek("ERROR: Error in writing to the end of the file (test_write_2)", fd);
    else
      handleFailureSeek("ERROR: Error in reading from the beginning of the file (test_write_2)", fd);
    return 0;
  }

  if (print)
    print_file_contents(fd, sizeof(expected2), "Content of the txt file after the second write:");

  // Third test: Go back 5 characters and read until the end of the file
  // This test checks whether SEEK_CUR with negative offset brings the file to the correct offset
  if (test_read_1(fd))
  {
    if (print) printf("SUCCESS! Successfully read exactly the last 5 characters from the file\n");
  }
  else
  {
    handleFailureSeek("ERROR: Error in reading exactly the last 5 characters of the file (test_read_1)", fd);
    return 0;
  }

  // Fourth test: write "Extra fresh data!\n" to the end of the file, then read to make sure it was received successfully
  // This test checks whether going too far forward (offset > file size) or too far back (offset < 0)
  // with WHENCE = SEEK_SET
  int res3;
  if ((res3 = test_write_3(fd)) == 1)
  {
    if (print) printf("SUCCESS! Successfully written \"Extra fresh data!\\n\" to the file\n");
  }
  else
  {
    if (res3 == 0)
      handleFailureSeek("ERROR: Error in writing to the end of the file (test_write_3)", fd);
    else
      handleFailureSeek("ERROR: Error in reading from the beginning of the file (test_write_3)", fd);
    return 0;
  }

  if (print)
    print_file_contents(fd, sizeof(expected3), "Content of the txt file after the third write:");

  printf("SUCCESS!! PASSED SEEK TETSTS\n");
  close(fd);
  return 1;
}