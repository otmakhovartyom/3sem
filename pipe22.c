#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("Incorrect number of arguments\n");
		return 0;
	}
	
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		perror("open fail");
	
	struct stat file_stat;
	stat(argv[1], &file_stat);
	unsigned int Size_of_file = file_stat.st_size;
//	printf("size = %u\n", Size_of_file);	
	
	char* buffer = (char*)malloc(Size_of_file);
	int reader = read(fd, buffer, Size_of_file);
	if (reader == -1)
		perror("read fail");
	
	size_t current = 0;
	int num_p = 0, n = 0, beg = 0, end = 0;
	
	for (size_t i = 0; i < Size_of_file; i++)
	{
		if (buffer[i] == '|')
			num_p++;
	}
	
	char*** arggv = (char***)calloc(num_p + 1, sizeof(char**));
	
	for (size_t i = 0; i <= num_p; i++, n = 0)
	{
		end = current;
		if (num_p == i)
		{
			while (current < Size_of_file - 1)
			{
				current++;
				if (buffer[current] == ' ')
					n++;
			}
			n++;
		}
		
		else
		{
			while (buffer[current] != '|')
			{
				current++;
				if (buffer[current] == ' ')
					n++;
			}
		
			while (buffer[current] == '|' || buffer[current] == ' ')
				current++;
		}
		
		
	
//		printf("current = %zu n = %d num_p = %d\n", current, n, num_p);
		arggv[i] = (char**)calloc(n+1, sizeof(char*));
	
		for (size_t j = 0; j < n; j++)
		{
			beg = end;
			while ((buffer[end] != ' ' && buffer[end] != '\n') && end < Size_of_file - 1)
				end++;
			
//			printf("j = %zu beg = %d end = %d\n", j, beg, end);
			arggv[i][j] = (char*)calloc(end - beg + 1, sizeof(char));
			
			for (size_t k = 0; k < end - beg; k++)
			{
				arggv[i][j][k] = buffer[beg + k];
			}
			while (buffer[end] == ' ')
				end++;
		}
		arggv[i][n] = NULL;
		
//		for (size_t j = 0; j < n; j++)
//		{
//			printf("%s\n", arggv[i][j]);
//		}
	}
	
	int pipefd[num_p][2];
	pid_t cpid;
		
	for (size_t i = 0; i < num_p; i++)
	{
		if (pipe(pipefd[i]) == -1)
		{
			perror("pipe fail");
			exit(EXIT_FAILURE);
		}
	}

	cpid = fork();
	if (cpid == -1)
	{
		perror("fork fail");
		exit(EXIT_FAILURE);
	}
	
	for (size_t i = 0; i < num_p; i++)
	{
		if (cpid == 0) /* Потомок читает из канала */
		{
			if (i == num_p - 1)
			{
				close(pipefd[i][1]); /* Закрывает неиспользуемый конец для записи */
//				while (read(pipefd[0], &buf, 1) > 0)
//					write(STDOUT_FILENO, &buf, 1);
//				write(STDOUT_FILENO, "\n", 1);

				dup2(pipefd[i][0], STDIN_FILENO);
				printf("\n");
				if (execvp(arggv[i + 1][0], arggv[i + 1]) == -1)
				{
					perror("exec fail");
					exit(EXIT_FAILURE);
				}
				
				close(pipefd[i][0]);
				_exit(EXIT_SUCCESS);
			}
			
			else
			{
				close(pipefd[i][1]);
				cpid = fork();
				if (cpid == -1)
				{
					perror("fork fail");
					exit(EXIT_FAILURE);
				}
			}

		}
	
		else /* Родитель пишет значение argv[1] в канал */
		{
			if (i == 0)
			{
				close(pipefd[i][0]); /* Закрывает неиспользуемый конец для чтения */
//				write(pipefd[1], argv[1], strlen(argv[1]));
//				close(pipefd[1]); /* Читатель видит EOF */
				
				dup2(pipefd[i][1], STDOUT_FILENO);
				execvp(arggv[i][0], arggv[i]);
				
				wait(NULL); /* Ожидание потомка */
				close(pipefd[i][1]);
				exit(EXIT_SUCCESS);
			}
			
			else
			{
				close(pipefd[i][0]);
				dup2(pipefd[i - 1][0], STDIN_FILENO);
				dup2(pipefd[i][1], STDOUT_FILENO);
//				printf("\n");
				if (execvp(arggv[i][0], arggv[i]) == -1)
				{
					perror("exec fail");
					exit(EXIT_FAILURE);
				}
				
				wait(NULL); /* Ожидание потомка */
				close(pipefd[i - 1][0]);
				close(pipefd[i][1]);
				exit(EXIT_SUCCESS);
			}
		}
	}

	free(buffer);
	close(fd);
	return 0;
}
