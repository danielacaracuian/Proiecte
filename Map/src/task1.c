#include <stdlib.h>
#include <string.h>
#include "../header.h"
#include <stdbool.h>
#include <limits.h>
#include "MinHeap.h"

Queue *BFS(Graph *g)
{
    Queue *result = createQueue();
    Queue *q = createQueue();
    int *visited = calloc(g->V, sizeof(int));

    for (int i = 0; i < g->V; i++)
    {
        if (!visited[i])
        {
            enqueue(result, NULL, i);
            enqueue(q, front(g->adj[i]), 0);

            while (!isQueueEmpty(q))
            {
                char *u = front(q);
                dequeue(q, false);

                int j;
                for (j = 0; j < g->V; j++)
                {
                    if (compare_node(u, front(g->adj[j])))
                    {
                        break;
                    }
                }
                if (!visited[j])
                {
                    ListNode *iter = succ_u(g, j);

                    while (iter != g->adj[j]->head)
                    {
                        enqueue(q, iter->name, 0);
                        iter = iter->prev;
                    }

                    visited[j] = true;
                }
            }
        }
    }
    free(visited);
    destroyQueue(q, false);
    return result;
}

int Kruskal(Graph *g, int pos)
{
    int total_cost = 0;
    APriQueue priQ = makeQueue(g->V);
    for (int i = 0; i < g->V; i++)
    {
        ItemType x;
        x.content.pos = i;
        x.content.u = front(g->adj[i]);
        if (i == pos)
        {
            x.prior = 0;
        }
        else
        {
            x.prior = INT_MAX;
        }
        insert(priQ, x);
    }

    while (priQ->size > 0)
    {
        ItemType node = removeMin(priQ);
        if (node.prior == INT_MAX) {
            continue;
        }

        total_cost += node.prior;

        ListNode* succ = succ_u(g, node.content.pos);
        
        while (succ != g->adj[node.content.pos]->head)
        {
            for (int i = 0; i < priQ->size; i++)
            {
                if (compare_node(priQ->elem[i].content.u, succ->name)) {
                    if (priQ->elem[i].prior > succ->w) {
                        priQ->elem[i].prior = succ->w;
                        siftUp(priQ, i);
                        break;
                    }
                }
            }
            
            succ = succ->prev;
        }
    }

    free(priQ->elem);
    free(priQ);

    return total_cost;
}