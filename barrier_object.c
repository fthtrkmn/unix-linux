#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE			50000000
#define NTHREADS		10

int comp(const void *pv1, const void *pv2);
void *thread_proc(void *param);
void merge(void);
int check(const int *nums);
void exit_sys_thread(const char *msg, int err);

pthread_barrier_t g_barrier;
int g_nums[SIZE];
int g_snums[SIZE];

int main(void)
{
	int result;
	int i;
	pthread_t tids[NTHREADS];
	clock_t start, stop;
	double telapsed;

	srand(time(NULL));
	for (i = 0; i < SIZE; ++i)
		g_nums[i] = rand();

	start = clock();

	if ((result = pthread_barrier_init(&g_barrier, NULL, NTHREADS)) != 0)
		exit_sys_thread("pthread_barrier_init", result);

	for (i = 0; i < NTHREADS; ++i)
		if ((result = pthread_create(&tids[i], NULL, thread_proc, (void *)i)) != 0)
			exit_sys_thread("pthread_create", result);

	for (i = 0; i < NTHREADS; ++i)
		if ((result = pthread_join(tids[i], NULL)) != 0)
			exit_sys_thread("pthread_join", result);

	pthread_barrier_destroy(&g_barrier);

	stop = clock();

	telapsed = (double)(stop - start) / CLOCKS_PER_SEC;

	printf("Total second with threaded sort: %f\n", telapsed);

	printf(check(g_snums) ? "Sorted\n" : "Not Sorted\n");

	start = clock();

	qsort(g_nums, SIZE, sizeof(int), comp);

	stop = clock();

	telapsed = (double)(stop - start) / CLOCKS_PER_SEC;

	printf("Total second with threaded sort: %f\n", telapsed);

	printf(check(g_nums) ? "Sorted\n" : "Not Sorted\n");

	return 0;
}

void exit_sys_thread(const char *msg, int err)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(err));
	exit(EXIT_FAILURE);
}

void *thread_proc(void *param)
{
	int part = (int)param;
	int result;

	qsort(g_snums + part * (SIZE / NTHREADS), SIZE / NTHREADS, sizeof(int), comp);

	if ((result = pthread_barrier_wait(&g_barrier)) && result != PTHREAD_BARRIER_SERIAL_THREAD)
		exit_sys_thread("pthread_barrier_wait", result);

	if (result == PTHREAD_BARRIER_SERIAL_THREAD)
		merge();

	return NULL;
}

int comp(const void *pv1, const void *pv2)
{
	const int *pi1 = (const int *)pv1;
	const int *pi2 = (const int *)pv2;

	return *pi1 - *pi2;
}

void merge(void)
{
	int indexes[NTHREADS];
	int min, min_index;
	int i, k;
	int partsize;

	partsize = SIZE / NTHREADS;

	for (i = 0; i < NTHREADS; ++i)
		indexes[i] = i * partsize;

	for (i = 0; i < SIZE; ++i) {
		min = indexes[0];
		min_index = 0;

		for (k = 1; k < NTHREADS; ++k)
			if (indexes[k] < (k + 1) * partsize && g_nums[indexes[k]] < min) {
				min = g_nums[indexes[k]];
				min_index = k;
			}
		g_snums[i] = min;
		++indexes[min_index];
	}
}

int check(const int *nums)
{
	int i;

	for (i = 0; i < SIZE - 1; ++i)
		if (nums[i] > nums[i + 1])
			return 0;

	return 1;
}
