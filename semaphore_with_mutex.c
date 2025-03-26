#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NTHREADS		10
#define NRESOURCES		3

typedef struct tagRESOURCES {
	int useflags[NRESOURCES];
	sem_t sem;
	pthread_mutex_t mutex;
} RESOURCES;

typedef struct tagTHREAD_INFO {
	pthread_t tid;
	char name[32];
	unsigned seed;
} THREAD_INFO;

void assign_resource(THREAD_INFO *ti);
void do_with_resource(THREAD_INFO *ti, int nresource);
void *thread_proc(void *param);
void exit_sys(const char *msg);
void exit_sys_errno(const char *msg, int eno);

RESOURCES g_resources;

int main(void)
{
	int result;
	THREAD_INFO *threads_info[NTHREADS];

	srand(time(NULL));

	for (int i = 0; i < NRESOURCES; ++i)
		g_resources.useflags[i] = 0;

	if ((sem_init(&g_resources.sem, 0, NRESOURCES)) == -1)
		exit_sys("sem_init");
	if ((result = pthread_mutex_init(&g_resources.mutex, NULL)) != 0)
		exit_sys_errno("pthread_mutex_init", result);

	for (int i = 0; i < NTHREADS; ++i) {
		if ((threads_info[i] = (THREAD_INFO *)malloc(sizeof(THREAD_INFO))) == NULL) {
			fprintf(stderr, "cannot allocate memory!...\n");
			exit(EXIT_FAILURE);
		}
		snprintf(threads_info[i]->name, 32, "Thread-%d", i + 1);
		threads_info[i]->seed = rand();
		if ((result = pthread_create(&threads_info[i]->tid, NULL, thread_proc, threads_info[i])) != 0)
			exit_sys_errno("pthread_create", result);
	}

	for (int i = 0; i < NTHREADS; ++i) {
		if ((result = pthread_join(threads_info[i]->tid, NULL)) != 0)
			exit_sys_errno("pthread_join", result);
		free(threads_info[i]);
	}

	if ((result = pthread_mutex_destroy(&g_resources.mutex)) != 0)
		exit_sys_errno("pthread_mutex_destroy", result);

	if (sem_destroy(&g_resources.sem) == -1)
		exit_sys("sem_destroy");

	return 0;
}

void assign_resource(THREAD_INFO *ti)
{
	int result;
	int i;

	if (sem_wait(&g_resources.sem) == -1)
		exit_sys("sem_wait");

	if ((result = pthread_mutex_lock(&g_resources.mutex)) != 0)
		exit_sys_errno("pthread_mutex_lock", result);

	for (i = 0; i < NRESOURCES; ++i)
		if (!g_resources.useflags[i]) {
			g_resources.useflags[i] = 1;
			break;
		}

	if ((result = pthread_mutex_unlock(&g_resources.mutex)) != 0)
		exit_sys_errno("pthread_mutex_unlock", result);

	printf("%s thread acquired resource \"%d\"\n", ti->name, i + 1);

	do_with_resource(ti, i + 1);

	if ((result = pthread_mutex_lock(&g_resources.mutex)) != 0)
		exit_sys_errno("pthread_mutex_lock", result);

	g_resources.useflags[i] = 0;

	if ((result = pthread_mutex_unlock(&g_resources.mutex)) != 0)
		exit_sys_errno("pthread_mutex_unlock", result);

	printf("%s thread released resource \"%d\"\n", ti->name, i + 1);

	if (sem_post(&g_resources.sem) == -1)
		exit_sys("sem_wait");

	usleep(rand_r(&ti->seed) % 10000);
}

void do_with_resource(THREAD_INFO *ti, int nresource)
{
	printf("%s doing something with resource \"%d\"\n", ti->name, nresource);

	usleep(rand_r(&ti->seed) % 500000);
}

void *thread_proc(void *param)
{
	THREAD_INFO *ti = (THREAD_INFO *)param;

	for (int i = 0; i < 10; ++i)
		assign_resource(ti);

	return NULL;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}

void exit_sys_errno(const char *msg, int eno)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(eno));

	exit(EXIT_FAILURE);
}
