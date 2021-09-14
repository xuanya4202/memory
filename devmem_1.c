#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef long int  uint64_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

int main(int argc, char **argv)
{
	void *map_base, *virt_addr;
	uint64_t read_result;
	uint64_t writeval = writeval; /* for compiler */
	off_t target;
	unsigned page_size, mapped_size, offset_in_page;
	int fd;
	unsigned width = 8 * sizeof(int);

	/* devmem ADDRESS [WIDTH [VALUE]] */
// TODO: options?
// -r: read and output only the value in hex, with 0x prefix
// -w: write only, no reads before or after, and no output
// or make this behavior default?
// Let's try this and see how users react.

	/* ADDRESS */
	int errno = 0;
	target = strtoull(argv[1], NULL, 0); /* allows hex, oct etc */

	/* WIDTH */
	if (argv[2]) {
		if (isdigit(argv[2][0]) || argv[2][1])
			width = atoi(argv[2]);
		else {
			//static const char bhwl[] ALIGN1 = "bhwl";
			//static const uint8_t sizes[] ALIGN1 = {
			static const char bhwl[]  = "bhwl";
			static const uint8_t sizes[] = {
				8 * sizeof(char),
				8 * sizeof(short),
				8 * sizeof(int),
				8 * sizeof(long),
				0 /* bad */
			};
			//width = strchrnul(bhwl, (argv[2][0] | 0x20)) - bhwl;
			width = 0;
			width = sizes[width];
		}
		/* VALUE */
		if (argv[3])
			writeval = strtoull(argv[3], NULL, 0);
	} else { /* argv[2] == NULL */
		/* make argv[3] to be a valid thing to fetch */
		argv--;
	}
	if (errno)
  {
    printf("errno %d\n", errno);
    return -1;
  }
	fd = open("/dev/mem", argv[3] ? (O_RDWR | O_SYNC) : (O_RDONLY | O_SYNC));
	mapped_size = page_size = 4096;
	offset_in_page = (unsigned)target & (page_size - 1);
	if (offset_in_page + width > page_size) {
		/* This access spans pages.
		 * Must map two pages to make it possible: */
		mapped_size *= 2;
	}
	map_base = mmap(NULL,
			mapped_size,
			argv[3] ? (PROT_READ | PROT_WRITE) : PROT_READ,
			MAP_SHARED,
			fd,
			target & ~(off_t)(page_size - 1));
	if (map_base == MAP_FAILED)
    return -1;

//	printf("Memory mapped at address %p.\n", map_base);

	virt_addr = (char*)map_base + offset_in_page;

	if (!argv[3]) {
		switch (width) {
		case 8:
			read_result = *(volatile uint8_t*)virt_addr;
			break;
		case 16:
			read_result = *(volatile uint16_t*)virt_addr;
			break;
		case 32:
			read_result = *(volatile uint32_t*)virt_addr;
			break;
		case 64:
			read_result = *(volatile uint64_t*)virt_addr;
			break;
		default:
      printf("bad width");
      return -1;
		}
//		printf("Value at address 0x%"OFF_FMT"X (%p): 0x%llX\n",
//			target, virt_addr,
//			(unsigned long long)read_result);
		/* Zero-padded output shows the width of access just done */
		printf("0x%0*llX\n", (width >> 2), (unsigned long long)read_result);
	} else {
		switch (width) {
		case 8:
			*(volatile uint8_t*)virt_addr = writeval;
			read_result = *(volatile uint8_t*)virt_addr;
			break;
		case 16:
			*(volatile uint16_t*)virt_addr = writeval;
			read_result = *(volatile uint16_t*)virt_addr;
			break;
		case 32:
			*(volatile uint32_t*)virt_addr = writeval;
			read_result = *(volatile uint32_t*)virt_addr;
			break;
		case 64:
			*(volatile uint64_t*)virt_addr = writeval;
			read_result = *(volatile uint64_t*)virt_addr;
			break;
		default:
			printf("bad width");
		}
		printf("Written 0x%llX; readback 0x%llX\n",
				(unsigned long long)writeval,
				(unsigned long long)read_result);
	}

		if (munmap(map_base, mapped_size) == -1)
			printf("munmap");
		close(fd);

	return 1;
}
