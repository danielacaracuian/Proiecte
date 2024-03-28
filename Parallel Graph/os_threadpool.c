#include "os_threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* === TASK === */

/* Creates a task that thread must execute */
os_task_t *task_create(void *arg, void (*f)(void *))
{
	os_task_t *task = (os_task_t *)malloc(sizeof(os_task_t));

	task->argument = arg;
	task->task = f;

	return task;
}

/* Add a new task to threadpool task queue */
void add_task_in_queue(os_threadpool_t *tp, os_task_t *t)
{
	os_task_queue_t *new_task =
		(os_task_queue_t *)malloc(sizeof(os_task_queue_t));

	new_task->task = t;
	new_task->next = NULL;
	pthread_mutex_lock(&tp->taskLock);

	os_task_queue_t *node = tp->tasks;

	if (node == NULL) {
		tp->tasks = new_task;
		pthread_mutex_unlock(&tp->taskLock);
		return;
	}

	while (node->next != NULL)
		node = node->next;

	node->next = new_task;
	pthread_mutex_unlock(&tp->taskLock);
}

/* Get the head of task queue from threadpool */
os_task_t *get_task(os_threadpool_t *tp)
{
	pthread_mutex_lock(&tp->taskLock);
	os_task_queue_t *head = tp->tasks;

	if (head == NULL) {
		pthread_mutex_unlock(&tp->taskLock);
		return NULL;
	}
	tp->tasks = tp->tasks->next;
	pthread_mutex_unlock(&tp->taskLock);
	os_task_t *task = head->task;

	free(head);

	return task;
}

/* === THREAD POOL === */

/* Initialize the new threadpool */
os_threadpool_t *threadpool_create(unsigned int nTasks, unsigned int nThreads)
{
	os_threadpool_t *tp = (os_threadpool_t *)malloc(sizeof(os_threadpool_t));

	tp->num_threads = nThreads;
	tp->threads = (pthread_t *)malloc(sizeof(pthread_t) * nThreads);
	tp->should_stop = 0;

	pthread_mutex_init(&tp->taskLock, NULL);
	for (int i = 0; i < nThreads; i++) {
		pthread_t thread;

		pthread_create(&thread, NULL, thread_loop_function, tp);
		tp->threads[i] = thread;
	}

	return tp;
}

/* Loop function for threads */
void *thread_loop_function(void *args)
{
	os_threadpool_t *tp = (os_threadpool_t *)args;

	while (!tp->should_stop) {
		os_task_t *task;

		task = get_task(tp);
		if (task == NULL)
			continue;
		task->task(task->argument);
		free(task);
	}

	return NULL;
}

/* Stop the thread pool once a condition is met */
void threadpool_stop(os_threadpool_t *tp,
					 int (*processingIsDone)(os_threadpool_t *))
{
	while (!processingIsDone(tp))
		;

	tp->should_stop = 1;

	for (int i = 0; i < tp->num_threads; i++)
		pthread_join(tp->threads[i], NULL);

	free(tp->tasks);
	free(tp->threads);
	free(tp);
}
