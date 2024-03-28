#include "../header.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void print_graph(Graph *g)
{
    for (int i = 0; i < g->V; i++)
    {
        ListNode *iter = g->adj[i]->head->prev;
        printf("%s : ", iter->name);
        iter = iter->prev;

        while (iter != g->adj[i]->head)
        {
            printf("%s, ", iter->name);
            iter = iter->prev;
        }
        printf("\n");
    }
}

Graph *create_graph(FILE *fd, int bidirectional)
{
    Graph *g = malloc(sizeof(Graph));
    g->depth = NULL;
    g->adj = NULL;
    check(g == NULL, "Graph creation");

    fscanf(fd, "%d %d\n", &g->V, &g->E);

    g->adj = malloc(sizeof(List *) * g->V);
    check(g->adj == NULL, "Graph Lists");

    for (int i = 0; i < g->V; i++)
    {
        g->adj[i] = createList();
    }

    for (int i = 0; i < g->E; i++)
    {
        char node1_buf[50] = {'\0'};
        char node2_buf[50] = {'\0'};
        int w = 0;
        fscanf(fd, "%s %s %d\n", node1_buf, node2_buf, &w);

        add_edge(g, node1_buf, node2_buf, w);
        if (bidirectional)
        {
            add_edge(g, node2_buf, node1_buf, w);
        }
    }

    if (!bidirectional)
    {
        g->depth = malloc(sizeof(int) * g->V);
        for (int i = 0; i < g->V; i++)
        {
            char node1_buf[50] = {'\0'};
            int depth = 0;
            fscanf(fd, "%s %d\n", node1_buf, &depth);

            for (int j = 0; j < g->V; j++)
            {
                if (compare_node(front(g->adj[j]), node1_buf))
                {
                    g->depth[j] = depth;
                }
            }
        }
        fscanf(fd, "%d\n", &g->kg_tr);
        print_graph(g);
        printf("===========\n");
    }
    return g;
}

void add_edge(Graph *g, char *name1, char *name2, int w)
{
    int len_n1 = strlen(name1);
    int len_n2 = strlen(name2);

    char *node1 = malloc(len_n1 + 1);
    check(node1 == NULL, "node1 alloc");
    memcpy(node1, name1, len_n1 + 1);

    char *node2 = malloc(len_n2 + 1);
    check(node2 == NULL, "node2 alloc");
    memcpy(node2, name2, len_n2 + 1);

    for (int j = 0; j < g->V; j++)
    {
        if (isListEmpty(g->adj[j]))
        {
            enqueue((Queue *)g->adj[j], node1, 0);
            enqueue((Queue *)g->adj[j], node2, w);
            break;
        }
        else if (compare_node(node1, front(g->adj[j])))
        {
            enqueue((Queue *)g->adj[j], node2, w);
            free(node1);
            break;
        }
    }

    for (int j = 0; j < g->V; j++)
    {
        if (isListEmpty(g->adj[j]))
        {
            node2 = malloc(len_n2 + 1);
            check(node2 == NULL, "node2 alloc");
            memcpy(node2, name2, len_n2 + 1);
            enqueue((Queue *)g->adj[j], node2, 0);
            break;
        }
        else if (compare_node(node2, front(g->adj[j])))
        {
            break;
        }
    }
}

void destory_graph(Graph *g)
{
    for (int i = 0; i < g->V; i++)
    {
        destroyList(g->adj[i], true);
    }
    if (g->depth != NULL)
    {
        free(g->depth);
    }
    free(g->adj);
    free(g);
}

bool compare_node(char *name1, char *name2)
{
    int len_1 = strlen(name1);
    int len_2 = strlen(name2);

    return (len_1 == len_2 && strncmp(name1, name2, len_1) == 0);
}

ListNode *succ_u(Graph *g, int i)
{
    return g->adj[i]->head->prev->prev;
}

int find_pos(Graph *g, char *name)
{
    for (int i = 0; i < g->V; i++)
    {
        if (compare_node(front(g->adj[i]), name))
        {
            return i;
        }
    }
    return -1;
}
