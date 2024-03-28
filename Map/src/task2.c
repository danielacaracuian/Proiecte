#include <stdlib.h>
#include <string.h>
#include "../header.h"
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include "MinHeap.h"

int *Dijkstra(Graph *g, int src_pos, int *p)
{
    float *result = malloc(sizeof(float) * g->V);

    int *dist = malloc(sizeof(int) * g->V);
    APriQueue priQ = makeQueue(g->V);

    for (int i = 0; i < g->V; i++)
    {
        ItemType x;
        x.content.pos = i;
        x.content.u = front(g->adj[i]);
        if (i == src_pos)
        {
            x.content.dist = 0;
            x.prior = 0;
        }
        else
        {
            x.content.dist = INT_MAX;
            x.prior = FLT_MAX;
        }
        result[i] = FLT_MAX;
        insert(priQ, x);
    }

    while (priQ->size > 0)
    {
        ItemType node = removeMin(priQ);
        result[node.content.pos] = node.prior;
        dist[node.content.pos] = node.content.dist;

        if (node.prior == INT_MAX)
        {
            continue;
        }

        ListNode *succ = succ_u(g, node.content.pos);

        while (succ != g->adj[node.content.pos]->head)
        {
            for (int i = 0; i < priQ->size; i++)
            {
                if (compare_node(priQ->elem[i].content.u, succ->name))
                {
                    int succ_pos_in_adj = 0;

                    for (int j = 0; j < g->V; j++)
                    {
                        if (compare_node(front(g->adj[j]), succ->name))
                        {
                            succ_pos_in_adj = j;
                            break;
                        }
                    }
                    float score = (succ->score + result[node.content.pos]);
                    if (priQ->elem[i].prior > score)
                    {
                        priQ->elem[i].prior = score;
                        priQ->elem[i].content.dist = succ->w + dist[node.content.pos];
                        p[succ_pos_in_adj] = node.content.pos;
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
    free(result);
    return dist;
}