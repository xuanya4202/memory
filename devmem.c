#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

int main(int argc, char **argv)
{
  unsigned char * map_base;
  off_t target;
  FILE *f;
  int n, fd;
  target = strtoul(argv[1], 0, 0);

  fd = open("/dev/mem", O_RDWR|O_SYNC);
  if (fd == -1)
  {
    printf("open failed!\n");
    return (-1);
  }
  map_base = mmap(NULL, 0xff, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x20000);
  if (map_base == 0)
  {
    printf("NULL pointer!\n");
  }
  else
  {
    printf("Successfull!\n");
  }

  void* virt_addr = map_base + (target & MAP_MASK);
  unsigned long read_result = *((unsigned char *)virt_addr);

  printf("Value at address 0x%X (%p): 0x%X\n", target, virt_addr, read_result);
 // unsigned long addr;
 // unsigned char content;
 // int i = 0;
 // for (;i < 0xff; ++i)
 // {
 //   addr = (unsigned long)(map_base + i);
 //   content = map_base[i];
 //   printf("address: 0x%lx   content 0x%x\t\t", addr, (unsigned int)content);
 //   map_base[i] = (unsigned char)i;
 //   content = map_base[i];
 //   printf("updated address: 0x%lx   content 0x%x\n", addr, (unsigned int)content);
 // }
  close(fd);
  munmap(map_base, 0xff);
  return (1);
}
