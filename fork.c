#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
	int opened = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
	pid_t pid = 0;
	int get = 0, getp = 0;
	char buffer[50];
	printf("%zu\n", sizeof("2 child: %d parent: %d\n"));
	FILE* fout = fopen(argv[1], "w");
	
	fprintf(fout, "%d %d\n", getpid(), getppid());
//	for (int i = 0; i < 1; i++)
//		pid = fork();

		switch (pid = fork())
		{
			case -1:
				perror("fork fail");
				exit(1);
			case 0:
				fprintf(fout, "1 child: %d parent: %d pid: %d\n", getpid(), getppid(), pid);
//				printf("1 child: %d parent: %d\n", getpid(), getppid());
//				printf("%s", buffer);
//				write(opened, buffer, sizeof(buffer));
			default:
				fprintf(fout, "2 child: %d parent: %d pparent: %d\n", pid, getpid(), getppid());
//				printf("2 child: %d parent: %d\n", pid, getpid());
//				printf("%s", buffer);
//				write(opened, buffer, sizeof(buffer));
		}
	
	close(opened);
	return 0;
}
