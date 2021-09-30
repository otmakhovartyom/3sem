#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>

/*struct pollfd{
	int fd;
	short events;
	short revents;
};*/

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Not enough arguments\n");
		return 0;
	}
	
	int count = argc - 1;
	
	struct pollfd test[count];
	int opened = 0, reader = 0, writer = 0;
	char symbol = 0;
	
	for (int i = 0; i < count; i++)
	{
		opened = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
		if (opened == -1)
			perror("open fail");
		test[i].fd = opened;
		test[i].events = POLLIN;
	}
	
	while (1)
	{
		poll(test, count, -1);
	
		for (int j = 0; j < count; j++)
		{
			if (test[j].revents > 0)
			{
				while ((reader = read(test[j].fd, &symbol, 1)) == 1)
					printf("%c", symbol);
			}
		}
	}
	
	return 0;
}
