#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		printf("Not enough arguments\n");
		return 0;
	}
	
	struct stat file_stat;
	stat(argv[1], &file_stat);
	unsigned int size = file_stat.st_size;
	
	int open1 = 0;
	int open2 = 0;
	int writer = 0;
	
	open1 = open(argv[1], O_RDWR);
	if (open1 == -1)
		perror("open1 fail");
	
	open2 = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
	if (open2 == -1)
		perror("open2 fail");
	
	void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, open1, 0);
	
	writer = write(open2, addr, size);
	if (writer == -1)
		perror("write fail");
	
	munmap(addr, size);
	close(open1);
	close(open2);
	return 0;
}
