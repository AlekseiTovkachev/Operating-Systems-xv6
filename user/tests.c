#include "kernel/types.h"
#include "user.h"
#include "kernel/fcntl.h"


void test1()
{
  // int fd;
  // char buffer[20];
  // int bytesRead;
  // // Create a new file
  //  fd = open("testfile.txt", O_CREATE | O_WRONLY);
  // if (fd < 0) {
  //   printf("Error creating the file\n");
  //   return;
  // }

  // // Write some data to the file
  // write(fd, "Hello, World!", 13);
  // close(fd);

  // // Reopen the file for reading
  // fd = open("testfile.txt", O_RDONLY);
  // if (fd < 0) {
  //   printf("Error opening the file\n");
  //   return;
  // }

  // // Seek to the beginning of the file
  // if (seek(fd, 0, SEEK_SET) < 0)
  //   printf("seek failed\n");

  // // Read and print the file contents
  // bytesRead = read(fd, buffer, sizeof(buffer) - 1);
  // if (bytesRead < 0) {
  //   printf("Error reading the file\n");
  //   return;
  // }
  // buffer[bytesRead] = '\0';
  // printf("File contents: %s\n", buffer);


  // if (seek(fd, 7, SEEK_SET) < 0)
  //   printf("seek failed\n");

  // // Read and print the file contents
  // bytesRead = read(fd, buffer, sizeof(buffer) - 1);
  // if (bytesRead < 0) {
  //   printf("Error reading the file\n");
  //   return;
  // }
  // buffer[bytesRead] = '\0';
  // printf("File contents: %s\n", buffer);

  // if (seek(fd, -6, SEEK_CUR) < 0)
  //   printf("seek failed\n");

  // // Read and print the file contents
  // bytesRead = read(fd, buffer, sizeof(buffer) - 1);
  // if (bytesRead < 0) {
  //   printf("Error reading the file\n");
  //   return;
  // }
  // buffer[bytesRead] = '\0';
  // printf("File contents: %s\n", buffer);

  // if (seek(fd, -10, SEEK_CUR) < 0)
  //   printf("seek failed\n");

  // // Read and print the file contents
  // bytesRead = read(fd, buffer, sizeof(buffer) - 1);
  // if (bytesRead < 0) {
  //   printf("Error reading the file\n");
  //   return;
  // }
  // buffer[bytesRead] = '\0';
  // printf("File contents: %s\n", buffer);

  // if (seek(fd, 30, SEEK_CUR) < 0)
  //   printf("seek failed\n");

  // if (seek(fd, -2, SEEK_CUR) < 0)
  //   printf("seek failed\n");


  // // Read and print the file contents
  // bytesRead = read(fd, buffer, sizeof(buffer) - 1);
  // if (bytesRead < 0) {
  //   printf("Error reading the file\n");
  //   return;
  // }
  // buffer[bytesRead] = '\0';
  // printf("File contents: %s\n", buffer);


  // close(fd);

  int fd;
  fd = open("random", O_RDWR);
  char buf[10];
  read(fd, &buf, 10);
  for (int i = 0; i < 10; i++)
  {
    printf("%x\n", (int)(buf[i]));
  }
  close(fd);

  return;
}


int main(void)
{
  test1();
  // test2();

  return 0;
}
