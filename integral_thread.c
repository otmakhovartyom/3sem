#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>

#define ull unsigned long long

double sum = 0;
double step = 0;
ull count_of_iteration = 0;

void* calculate_part(void * data)
{
	double value = *(double*)data;
	double mini_sum = 0;
	//printf("start value = %f\n", value);
	for (ull i = 0; i < count_of_iteration; i++)
	{
		mini_sum += (value / sin(value)) * step;
		value += step;
	}
	
	sum += mini_sum;
	//printf("end value = %f\n", value);
}

int main (int argc, char ** argv)
{

	if (argc != 3)
	{
		printf("Wrong number of arguments\n");
		return 0;
	}

	ull N = atoll(argv[1]);
	ull K = atoll(argv[2]);

	//printf("N = %llu\n", N);
	//printf("K = %llu\n", K);

	double down_border = M_PI / 4;
	double high_border = M_PI / 2;
	double between = high_border - down_border;
	step = between / K;

	//printf("start %f\n", down_border);
	//printf("end %f\n", high_border);

	pthread_t * tid = calloc(N, sizeof(pthread_t));

	count_of_iteration = K / N;
	ull number_step = 0;

	double * data = calloc(N, sizeof(double));

	double position = down_border;

	int trash;
	ull number_of_thread = 0;

	//printf("Count of iteration = %llu\n", count_of_iteration);

	for (ull j = 0; j < N; j++)
	{
		if (number_step == K)
			break;
		
		data[j] = position;
		position += between / N;
		int ret_val = pthread_create(tid + j, NULL, calculate_part, data + j);
		
		if (ret_val < 0)
			perror("OH NO!!!");
		
		number_of_thread++;
		number_step++;
	}
	
	for (ull j = 0; j < number_of_thread; j++)
		pthread_join(tid[j], (void**)&trash);
	
	number_of_thread = 0;
	
	printf("res = %f\n", sum);	

	free(tid);

	return 0;
}
