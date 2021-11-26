#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void my_printf(char* format, ...)
{
	int d;
	char c;
	char* s;
	va_list factor;
	va_start(factor, format);
	for (char* cur = format; *cur; cur++)
	{
		if(*cur != '%')
		{
			write(STDOUT_FILENO, cur, sizeof(char));
			continue;
		}
		
		switch(*(++cur))
		{
			case 's':
				s = va_arg(factor, char *);
				write(STDOUT_FILENO, s, strlen(s));
				break;
			case 'c':
				c = va_arg(factor, int);
				write(STDOUT_FILENO, &c, sizeof(char));
				break;
			case 'd':
				d = va_arg(factor, int);
				sprintf(s, "%d", d);
				write(STDOUT_FILENO, s, strlen(s));
				break;
			default:
				write(STDOUT_FILENO, cur, sizeof(char));
		}
	}
	
	va_end(factor);
}

int main()
{
	char s[20]= "Britain.\0";
	char c = 'a';
	int d = 8982;
	my_printf("London is %c capital of the Great %s\tPopulation: %dk\n", c, s, d);
	return 0;
}
