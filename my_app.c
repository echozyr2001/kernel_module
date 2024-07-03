#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/my_device"
#define DEVICE_NUM 100
// IO控制命令
#define IOCTL_ALLOC_MEM _IOR(DEVICE_NUM, 0, size_t)
#define IOCTL_WRITE_DATA _IOW(DEVICE_NUM, 1, char *)
#define IOCTL_READ_DATA _IOR(DEVICE_NUM, 2, char *)

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <input_file> <output_file>\n", argv[0]);
    return -1;
  }

  // 打开设备
  int fd = open(DEVICE_PATH, O_RDWR);
  if (fd < 0) {
    printf("Failed to open device\n");
    return -1;
  }

  // 打开输入文件
  FILE *file = fopen(argv[1], "r");
  if (!file) {
    printf("Failed to open input file\n");
  }

  // 获取文件大小
  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // 将输入文件内容储存在buffer中
  char *buffer = malloc(size);
  fread(buffer, 1, size, file);
  fclose(file);

  // 内存分配
  ioctl(fd, IOCTL_ALLOC_MEM, size);
  // 用户数据写入内核
  ioctl(fd, IOCTL_WRITE_DATA, buffer);

  // 清空buffer
  memset(buffer, 0, size);
  // 内核数据写入用户态
  ioctl(fd, IOCTL_READ_DATA, buffer);

  // 打开输出文件并写入数据
  file = fopen(argv[2], "w");
  if (!file) {
    printf("Filed to open output file\n");
    return -1;
  }
  fwrite(buffer, 1, size, file);
  fclose(file);

  // 释放内存并关闭设备
  free(buffer);
  close(fd);

  return 0;
}
