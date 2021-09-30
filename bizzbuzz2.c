#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 536870911

unsigned int changetext (char* buffer1, char* buffer2, size_t size);
void insert (int digit, int i, int* j, char* buffer1, char* buffer2);
void insertbizz (int* j, char* buffer2);
void insertbuzz (int* j, char* buffer2);

int main (int argc, char** argv)
{
	if (argc < 3)
	{
		printf("Not enough arguments\n");
		return 0;
	}
	
	struct stat file_stat;
	stat(argv[1], &file_stat);
	size_t Size_of_file1 = file_stat.st_size;
	size_t Size_of_file2 = 0;
//	printf("Size_of_file1 = %zu\n", Size_of_file1);
	
	int open1 = 0;
	int open2 = 0;
	int reader = 0;
	int writer = 0;
	int n_of_r = 0;
	int size = MAX - 1000;
	
	char* buffer1;
	char* buffer2;
	
	open1 = open(argv[1], O_RDONLY);
//	printf("op1-%d\n", open1);
	if (open1 == -1)
		perror("open1 fail");
		
	open2 = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
//	printf("op2-%d\n", open2);
	if (open2 == -1)
		perror("open2 fail");
	
	if (Size_of_file1 <= MAX)
	{
		buffer1 = calloc(Size_of_file1, 1);
		buffer2 = calloc(Size_of_file1, 4);
	
		reader = read(open1, buffer1, Size_of_file1);
//		printf("re-%d\n", reader);
		if (reader == -1)
		{
			perror("read fail");
			return 0;
		}	
	
//		printf("%s\n", buffer1);
		Size_of_file2 = changetext(buffer1, buffer2, Size_of_file1);
//		printf("Size_of_file2 = %zu\n", Size_of_file2);
//		printf("%s\n", buffer2);
	
		writer = write(open2, buffer2, Size_of_file2);
//		printf("wr-%d\n", writer);
		if (writer == -1)
			perror("write fail");
	}
	
	else
	{
		n_of_r = Size_of_file1 / MAX;
//		printf("%d-too big file\n", n_of_r);
		buffer1 = calloc(MAX, 1);
		buffer2 = calloc(MAX, 4);
		for (size_t r = 0; r <= n_of_r; r++)
		{
			reader = read(open1, buffer1, size);
			printf("(%zu)re-%d\n", r, reader);
			size = reader;
			if (reader == -1)
			{
				perror("read fail");
				return 0;
			}	
			while (buffer1[size - 1] >= '0' && buffer1[size - 1] <= '9')
			{
				read(open1, buffer1, 1);
				size++;
			}
	
//			printf("%s\n", buffer1);
			Size_of_file2 = changetext(buffer1, buffer2, size);
			printf("Size_of_file2 = %zu\n", Size_of_file2);
//			printf("%s\n", buffer2);
	
			writer = write(open2, buffer2, Size_of_file2);
			printf("wr-%d\n", writer);
			if (writer == -1)
				perror("write fail");
				
			size = MAX - 1000;
		}
	}
	
	free(buffer1);
	free(buffer2);
	close(open1);
	close(open2);
	return 0;
}

unsigned int changetext (char* buffer1, char* buffer2, size_t size)
{
	int digit = 0, number = 0, j = 0;
	_Bool point = 0, letter = 0, zero = 0, last = 0;
	size_t over = 0;
	
	for (int i = 0; i < size; i++)
	{
//		printf("%c-", buffer1[i]);
		if (buffer1[i] > '9' || buffer1[i] < '0')
		{
			//printf("huu");
			if (buffer1[i] != '-' &&  buffer1[i] != '+' &&  buffer1[i] != '.' &&  buffer1[i] != ',')
			{
				if (buffer1[i] == ' ' || buffer1[i] == '\n' || buffer1[i] == '\t')
				{
					if (digit > 0 || (number > 0 || zero == 1))
					{
						if ((number % 3 != 0 && !last) || (digit > 0 && number == 0 && zero == 0))
						{
							insert(digit, i, &j, buffer1, buffer2);
						}
						
						else
						{
							over = over + 4 * ((number % 3 == 0 || zero == 1) + (last == 1 || zero == 1)) - digit;
//							printf("OVER%d %d %zu--", 4 * ((number % 3 == 0) + (number % 5 == 0)), digit, over);
							if (last == 1 || zero == 1)
								insertbizz(&j, buffer2);
							if (number % 3 == 0 || zero == 1)
								insertbuzz(&j, buffer2);
						}
						
						buffer2[j] = buffer1[i];
						j++;
						digit = 0;
						number = 0;
						point = 0;
						letter = 0;
						zero = 0;
						last = 0;
					}
					
					else
					{
						buffer2[j] = buffer1[i];
						j++;
						digit = 0;
						point = 0;
						letter = 0;
					}
				}
				
				else
				{
					if (digit > 0 || number > 0 || zero == 1)
						insert(digit, i, &j, buffer1, buffer2);
					buffer2[j] = buffer1[i];
					j++;
					digit = 0;
					number = 0;
					point = 0;
					letter = 1;
					zero = 0;
					last = 0;
				}
			}
			
			else
			{
				if (buffer1[i] == '-' || buffer1[i] == '+')
				{
					if (digit == 0 && !letter)
						digit++;
					
					else
					{
						if (digit > 0)
							insert(digit, i, &j, buffer1, buffer2);
						buffer2[j] = buffer1[i];
						j++;
						digit = 0;
						number = 0;
						point = 0;
						letter = 1;
						zero = 0;
						last = 0;
					}
				}
				
				else
				{
					if ((number > 0 || zero == 1) && !point)
					{
						digit++;
						point = 1;
					}
					
					else
					{
						if (digit > 0)
							insert(digit, i, &j, buffer1, buffer2);
						buffer2[j] = buffer1[i];
						j++;
						digit = 0;
						number = 0;
						point = 0;
						letter = 1;
						zero = 0;
						last = 0;						
					}
				}
			}
		}
		
		else
		{
//			printf("uu(%d)", (int)(buffer1[i] - '0'));
			if (!point && !letter)
			{
				if (zero == 1 && buffer1[i] != '0')
					zero = 0;
				
				if (number == 0 && buffer1[i] == '0')
					zero = 1;
				
				number += (int)(buffer1[i] - '0');
				
				if (buffer1[i] == '0' || buffer1[i] == '5')
					last = 1;
				digit++;
			}
			
			else
			{
				if (letter)
				{
					buffer2[j] = buffer1[i];
					j++;
				}
			
				else
				{
					if (buffer1[i] == '0')
					{
						digit++;
					}
					
					else
					{
						insert(digit, i, &j, buffer1, buffer2);
						buffer2[j] = buffer1[i];
						j++;
						digit = 0;
						number = 0;
						point = 0;
						letter = 1;
						zero = 0;
						last = 0;
						
					}
				}
			}
		}
			
	}
	return (size + over);
}

void insert (int digit, int i, int* j, char* buffer1, char* buffer2)
{
	while (digit > 0)
	{
		buffer2[*j] = buffer1[i - digit];
		(*j)++;
		digit--;
	}
}

void insertbizz (int* j, char* buffer2)
{
	buffer2[*j] = 'b';
	(*j)++;
	buffer2[*j] = 'i';
	(*j)++;
	buffer2[*j] = 'z';
	(*j)++;
	buffer2[*j] = 'z';
	(*j)++;
}

void insertbuzz (int* j, char* buffer2)
{
	buffer2[*j] = 'b';
	(*j)++;
	buffer2[*j] = 'u';
	(*j)++;
	buffer2[*j] = 'z';
	(*j)++;
	buffer2[*j] = 'z';
	(*j)++;
}
