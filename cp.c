#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		printf("Not enough arguments\n");
		return 0;
	}
	
	struct stat file_stat;
	stat(argv[1], &file_stat);
	unsigned int Size_of_file = file_stat.st_size;
//	printf("Size_of_file = %u\n", Size_of_file);
	
	int open1 = 0;
	int open2 = 0;
	int reader = 0;
	int writer = 0;
	
	void * buffer;
	buffer = malloc(Size_of_file);
	
	open1 = open(argv[1], O_RDONLY);
//	printf("op1-%d\n", open1);
	if (open1 == -1)
		perror("open1 fail");
	
	open2 = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
//	printf("op2-%d\n", open2);
	if (open2 == -1)
		perror("open2 fail");
	
	reader = read(open1, buffer, Size_of_file);
//	printf("re-%d\n", reader);
	if (reader == -1)
		perror("read fail");
	
	writer = write(open2, buffer, Size_of_file);
//	printf("wr-%d\n", writer);
	if (writer == -1)
		perror("write fail");
		
	free(buffer);
	close(open1);
	close(open2);
	return 0;
}
