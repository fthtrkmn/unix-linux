#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

void exit_sys_errno(const char *msg, int err);
void *thread_proc1(void *param);
void *thread_proc2(void *param);
void *thread_proc3(void *param);
void *thread_proc4(void *param);

pthread_rwlock_t g_rwlock = PTHREAD_RWLOCK_INITIALIZER;

int main(void)
{
	int result;
	pthread_t tid1, tid2, tid3, tid4;

	if ((result = pthread_create(&tid1, NULL, thread_proc1, NULL)) != 0)
		exit_sys_errno("pthread_create", result);

	if ((result = pthread_create(&tid2, NULL, thread_proc2, NULL)) != 0)
		exit_sys_errno("pthread_create", result);

	if ((result = pthread_create(&tid3, NULL, thread_proc3, NULL)) != 0)
		exit_sys_errno("pthread_create", result);

	if ((result = pthread_create(&tid4, NULL, thread_proc4, NULL)) != 0)
		exit_sys_errno("pthread_create", result);

	if ((result = pthread_join(tid1, NULL)) != 0)
		exit_sys_errno("pthread_join", result);

	if ((result = pthread_join(tid2, NULL)) != 0)
		exit_sys_errno("pthread_join", result);

	if ((result = pthread_join(tid3, NULL)) != 0)
		exit_sys_errno("pthread_join", result);

	if ((result = pthread_join(tid4, NULL)) != 0)
		exit_sys_errno("pthread_join", result);

	pthread_rwlock_destroy(&g_rwlock);

	return 0;
}

void exit_sys_errno(const char *msg, int err)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(err));
	exit(EXIT_FAILURE);
}

void *thread_proc1(void *param)
{
	int result;
	int seedval;

	seedval = (unsigned int)time(NULL) + 12345;

	for (int i = 0; i < 10; ++i) {
		usleep(rand_r(&seedval) % 300000);

		if ((result = pthread_rwlock_rdlock(&g_rwlock)) != 0)
			exit_sys_errno("pthread_rwlock_rdlock", result);

		printf("thread1 ENTERS to critical section for READING...\n");

		usleep(rand_r(&seedval) % 300000);

		printf("thread1 EXITS from critical section...\n");

		if ((result = pthread_rwlock_unlock(&g_rwlock)) != 0)
			exit_sys_errno("pthread_rwlock_unlock", result);
	}

	return NULL;
}

void *thread_proc2(void *param)
{
	int result;
	int seedval;

	seedval = (unsigned int)time(NULL) + 23456;

	for (int i = 0; i < 10; ++i) {
		usleep(rand_r(&seedval) % 300000);

		if ((result = pthread_rwlock_rdlock(&g_rwlock)) != 0)
			exit_sys_errno("pthread_rwlock_rdlock", result);

		printf("thread2 ENTERS to critical section for READING...\n");

		usleep(rand_r(&seedval) % 300000);

		printf("thread2 EXITS from critical section...\n");

		if ((result = pthread_rwlock_unlock(&g_rwlock)) != 0)
			exit_sys_errno("pthread_rwlock_unlock", result);
	}

	return NULL;
}

void *thread_proc3(void *param)
{
	int result;
	int seedval;

	seedval = (unsigned int)time(NULL) + 35678;

	for (int i = 0; i < 10; ++i) {
		usleep(rand_r(&seedval) % 300000);

		if ((result = pthread_rwlock_wrlock(&g_rwlock)) == -1)
			exit_sys_errno("pthread_rwlock_wrlock", result);

		printf("thread3 ENTERS to critical section for WRITING...\n");

		usleep(rand_r(&seedval) % 300000);

		printf("thread3 EXITS from critical section...\n");

		if ((result = pthread_rwlock_unlock(&g_rwlock)) == -1)
			exit_sys_errno("pthread_rwlock_unlock", result);
	}

	return NULL;
}

void *thread_proc4(void *param)
{
	int result;
	int seedval;

	seedval = (unsigned int)time(NULL) + 356123;

	for (int i = 0; i < 10; ++i) {
		usleep(rand_r(&seedval) % 300000);

		if ((result = pthread_rwlock_wrlock(&g_rwlock)) == -1)
			exit_sys_errno("pthread_rwlock_wrlock", result);

		printf("thread4 ENTERS to critical section for WRITING...\n");

		usleep(rand_r(&seedval) % 300000);

		printf("thread4 EXITS from critical section...\n");

		if ((result = pthread_rwlock_unlock(&g_rwlock)) == -1)
			exit_sys_errno("pthread_rwlock_unlock", result);
	}

	return NULL;
}
