#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "os_graph.h"
#include "os_list.h"
#include "os_threadpool.h"

#define MAX_TASK 100
#define MAX_THREAD 4

int sum;
os_graph_t *graph;
os_threadpool_t *tp;
pthread_mutex_t sum_lock;
pthread_mutex_t visited_lock;
int curr_nodes;

void processNode(void *node_ptr)
{
	os_node_t *node = (os_node_t *)node_ptr;

	pthread_mutex_lock(&sum_lock);
	sum += node->nodeInfo;
	curr_nodes++;
	pthread_mutex_unlock(&sum_lock);

	for (int i = 0; i < node->cNeighbours; i++) {
		pthread_mutex_lock(&visited_lock);
		if (graph->visited[node->neighbours[i]] == 0) {
			graph->visited[node->neighbours[i]] = 1;
			os_task_t *task =
				task_create(graph->nodes[node->neighbours[i]], processNode);

			add_task_in_queue(tp, task);
		}
		pthread_mutex_unlock(&visited_lock);
	}
}

void traverse_graph(void)
{
	for (int i = 0; i < graph->nCount; i++) {
		pthread_mutex_lock(&visited_lock);
		if (graph->visited[i] == 0) {
			graph->visited[i] = 1;
			os_task_t *task = task_create(graph->nodes[i], processNode);

			add_task_in_queue(tp, task);
		}
		pthread_mutex_unlock(&visited_lock);
	}
}

int processingIsDone(os_threadpool_t *tp)
{
	if (curr_nodes == graph->nCount)
		return 1;

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: ./main input_file\n");
		exit(1);
	}

	FILE *input_file = fopen(argv[1], "r");

	if (input_file == NULL) {
		printf("[Error] Can't open file\n");
		return -1;
	}

	graph = create_graph_from_file(input_file);
	if (graph == NULL) {
		printf("[Error] Can't read the graph from file\n");
		return -1;
	}

	tp = threadpool_create(MAX_TASK, MAX_THREAD);
	pthread_mutex_init(&sum_lock, NULL);
	pthread_mutex_init(&visited_lock, NULL);

	traverse_graph();

	threadpool_stop(tp, processingIsDone);

	printf("%d", sum);

	pthread_mutex_destroy(&sum_lock);
	pthread_mutex_destroy(&visited_lock);

	return 0;
}
