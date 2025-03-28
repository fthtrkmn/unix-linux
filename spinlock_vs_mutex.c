
/* spin.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_COUNT		100000000

void *thread_proc1(void *param);
void *thread_proc2(void *param);
void exit_sys_errno(const char *msg, int eno);

pthread_spinlock_t g_spinlock;
int g_count;

int main(void)
{
	pthread_t tid1, tid2;
	int result;

	if ((result = pthread_spin_init(&g_spinlock, 0)) != 0)
		exit_sys_errno("pthread_spin_init", result);

	if ((result = pthread_create(&tid1, NULL, thread_proc1, NULL)) != 0)
		exit_sys_errno("pthread_create", result);

	if ((result = pthread_create(&tid2, NULL, thread_proc2, NULL)) != 0)
		exit_sys_errno("pthread_create", result);

	if ((result = pthread_join(tid1, NULL)) != 0)
		exit_sys_errno("pthread_join", result);

	if ((result = pthread_join(tid2, NULL)) != 0)
		exit_sys_errno("pthread_join", result);

	if ((result = pthread_spin_destroy(&g_spinlock)) != 0)
		exit_sys_errno("pthread_spin_destroy", result);

	printf("%d\n", g_count);

	return 0;
}

void *thread_proc1(void *param)
{
	int result;

	for (int i = 0; i < MAX_COUNT; ++i) {
		if ((result = pthread_spin_lock(&g_spinlock)) != 0)
			exit_sys_errno("pthread_spin_lock", result);

		++g_count;

		if ((result = pthread_spin_unlock(&g_spinlock)) != 0)
			exit_sys_errno("pthread_spin_unlock", result);
	}

	return NULL;
}

void *thread_proc2(void *param)
{
	int result;

	for (int i = 0; i < MAX_COUNT; ++i) {
		if ((result = pthread_spin_lock(&g_spinlock)) != 0)
			exit_sys_errno("pthread_spin_lock", result);

		++g_count;

		if ((result = pthread_spin_unlock(&g_spinlock)) != 0)
			exit_sys_errno("pthread_spin_unlock", result);
	}

	return NULL;
}

void exit_sys_errno(const char *msg, int eno)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(eno));

	exit(EXIT_FAILURE);
}

/* mutex.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_COUNT		100000000

void *thread_proc1(void *param);
void *thread_proc2(void *param);
void exit_sys_errno(const char *msg, int eno);

pthread_mutex_t g_mutex;

int g_count = 0;

int main(void)
{
	pthread_t tid1, tid2;
	int result;

	if ((result = pthread_mutex_init(&g_mutex, NULL)) != 0)
		exit_sys_errno("pthread_mutex_init", result);

	if ((result = pthread_create(&tid1, NULL, thread_proc1, NULL)) != 0)
		exit_sys_errno("pthread_create", result);

	if ((result = pthread_create(&tid2, NULL, thread_proc2, NULL)) != 0)
		exit_sys_errno("pthread_create", result);

	if ((result = pthread_join(tid1, NULL)) != 0)
		exit_sys_errno("pthread_join", result);

	if ((result = pthread_join(tid2, NULL)) != 0)
		exit_sys_errno("pthread_join", result);

	printf("%d\n", g_count);

	if ((result = pthread_mutex_destroy(&g_mutex)) != 0)
		exit_sys_errno("pthread_mutex_destroy", result);

	return 0;
}

void *thread_proc1(void *param)
{
	int result;

	for (int i = 0; i < MAX_COUNT; ++i) {
		if ((result = pthread_mutex_lock(&g_mutex)) != 0)
			exit_sys_errno("pthread_mutex_lock", result);

		++g_count;

		if ((result = pthread_mutex_unlock(&g_mutex)) != 0)
			exit_sys_errno("pthread_mutex_unlock", result);
	}

	return NULL;
}

void *thread_proc2(void *param)
{
	int result;

	for (int i = 0; i < MAX_COUNT; ++i) {
		if ((result = pthread_mutex_lock(&g_mutex)) != 0)
				exit_sys_errno("pthread_mutex_lock", result);

			++g_count;

			if ((result = pthread_mutex_unlock(&g_mutex)) != 0)
				exit_sys_errno("pthread_mutex_unlock", result);
	}

	return NULL;
}

void exit_sys_errno(const char *msg, int eno)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(eno));

	exit(EXIT_FAILURE);
}
