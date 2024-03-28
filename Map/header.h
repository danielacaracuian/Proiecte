/*
 * Util.h
 *
 *  Created on: May 3, 2016
 *      Author: dan
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <stdbool.h>

typedef struct ListNode{
	char* name;
    int w;
    float score;
	struct ListNode *next;
	struct ListNode *prev;
}ListNode;

typedef struct List{
	ListNode* head;
}List;

typedef List Stack;
typedef List Queue;

List* createList(void);
void destroyList(List* list, int to_destroy_node_info);
int isListEmpty(List *list);

Stack* createStack(void);
void destroyStack(Stack* stack,int to_destroy_node_info);
void push(Stack *stack, char *name, int w);
void pop(Stack* stack);
int isStackEmpty(Stack* stack);
char* top(Stack *stack);


Queue* createQueue(void);
void destroyQueue(Queue* queue,int to_destroy_node_info);
void enqueue(Queue *queue,char* name, int w);
void dequeue(Queue* queue, int to_destory_node_info);
int isQueueEmpty(Queue *queue);
char* front(Queue *queue);


void check(int expr, char *message);
int compare_int(const void *a, const void *b);

typedef struct graph
{
    int V;
    int E;
    List **adj;
    int *depth;
    int kg_tr;
} Graph;

Graph *create_graph(FILE *fd, int bidirectional);
void destory_graph(Graph *g);
bool compare_node(char *name1, char *name2);
ListNode *succ_u(Graph *g, int i);
void add_edge(Graph *g, char *name1, char *name2, int w);
void print_graph(Graph *g);
int find_pos(Graph *g, char *name);

int Kruskal(Graph *g, int pos);
Queue *BFS(Graph *g);

int *Dijkstra(Graph* g, int src_pos, int*p);

#endif /* UTIL_H_ */
