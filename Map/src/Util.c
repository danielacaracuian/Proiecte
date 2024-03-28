#include "../header.h"
#include <stdlib.h>

List *createList(void)
{
	List *newList = (List *)malloc(sizeof(List));
	newList->head = (ListNode *)malloc(sizeof(ListNode));
	newList->head->next = newList->head->prev = newList->head;
	return newList;
}
void destroyList(List *list, int to_destroy_node_info)
{
	ListNode *it = list->head->next;
	while (it != list->head)
	{
		ListNode *aux = it;
		it = it->next;
		if (to_destroy_node_info)
			free(aux->name);
		free(aux);
	}
	free(list->head);
	free(list);
}
int isListEmpty(List *list)
{
	return list->head->next == list->head;
}

Stack *createStack(void)
{
	return createList();
}
void destroyStack(Stack *stack, int to_destroy_node_info)
{
	destroyList(stack, to_destroy_node_info);
}
void push(Stack *stack, char *name, int w)
{
	ListNode *newNode = (ListNode *)malloc(sizeof(ListNode));
	newNode->name = name;
	newNode->w = w;

	newNode->next = stack->head->next;
	newNode->prev = stack->head;

	stack->head->next->prev = newNode;
	stack->head->next = newNode;
}

void pop(Stack *stack)
{
	ListNode *aux = stack->head->next;
	stack->head->next = stack->head->next->next;
	stack->head->next->prev = stack->head;
	free(aux->name);
	free(aux);
}

int isStackEmpty(Stack *stack)
{
	return isListEmpty(stack);
}

char *top(Stack *stack)
{
	return stack->head->next->name;
}

Queue *createQueue(void)
{
	return createList();
}

void destroyQueue(Queue *queue, int to_destroy_node_info)
{
	destroyList(queue, to_destroy_node_info);
}

void enqueue(Queue *queue, char *name, int w)
{
	push(queue, name, w);
}
void dequeue(Queue *queue, int to_destory_node_info)
{
	ListNode *aux = queue->head->prev;
	queue->head->prev = queue->head->prev->prev;
	queue->head->prev->next = queue->head;
	if (to_destory_node_info)
		free(aux->name);
	free(aux);
}

int isQueueEmpty(Queue *queue)
{
	return isListEmpty(queue);
}

char *front(Queue *queue)
{
	return queue->head->prev->name;
}
