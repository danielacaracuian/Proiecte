#include <stdlib.h>
#include <string.h>
#include "header.h"
#include <limits.h>
#include <stdbool.h>

void task1(Graph *g)
{
    Queue *enter_nodes = BFS(g);
    ListNode *iter = enter_nodes->head->prev;
    int count = 0;
    while (iter != enter_nodes->head)
    {
        count++;
        iter = iter->prev;
    }
    int *total_cost = malloc(sizeof(int) * count);

    iter = enter_nodes->head->prev;
    for (int i = 0; i < count; i++)
    {
        total_cost[i] = Kruskal(g, iter->w);
        iter = iter->prev;
    }
    destroyQueue(enter_nodes, false);

    FILE *fd = fopen("tema3.out", "w");

    fprintf(fd, "%d\n", count);
    qsort(total_cost, count, sizeof(int), compare_int);
    for (int i = 0; i < count; i++)
    {
        fprintf(fd, "%d\n", total_cost[i]);
    }
    fclose(fd);
    free(total_cost);
}

void task2(Graph *g)
{
    int pos_insula = find_pos(g, "Insula"), pos_corabie = find_pos(g, "Corabie");

    FILE *fd = fopen("tema3.out", "w");
    int *p = malloc(sizeof(int) * g->V);
    memset(p, 0, g->V * sizeof(int));

    for (int i = 0; i < g->V; i++)
    {
        ListNode *succ = succ_u(g, i);
        while (succ != g->adj[i]->head)
        {
            int v_pos = find_pos(g, succ->name);
            succ->score = (float)((float)succ->w / (float)g->depth[v_pos]);
            succ = succ->prev;
        }
    }

    int *result_c = Dijkstra(g, pos_corabie, p);
    int *result_i = Dijkstra(g, pos_insula, p);

    if (result_i[pos_corabie] == INT_MAX)
    {
        fprintf(fd, "Echipajul nu poate transporta comoara inapoi la corabie\n");
    }
    else if (result_c[pos_insula] == INT_MAX)
    {
        fprintf(fd, "Echipajul nu poate ajunge la insula\n");
    }
    else
    {
        Stack *s = createStack();
        int max_kg = INT_MAX;
        int iter = pos_corabie;
        while (iter != pos_insula)
        {
            push(s, NULL, iter);
            if (g->depth[iter] < max_kg && iter != pos_corabie)
            {
                max_kg = g->depth[iter];
            }
            iter = p[iter];
        }
        push(s, NULL, iter);

        ListNode *iter_s = s->head->next;
        while (iter_s != s->head)
        {
            int pos = iter_s->w;
            fprintf(fd, "%s ", front(g->adj[pos]));
            iter_s = iter_s->next;
        }
        destroyStack(s, false);

        fprintf(fd, "\n%d\n", result_i[pos_corabie]);
        fprintf(fd, "%d\n", max_kg);
        int nr_drumuri = g->kg_tr / max_kg;
        fprintf(fd, "%d\n", g->kg_tr % max_kg == 0 ? nr_drumuri : nr_drumuri + 1);
    }

    free(p);
    free(result_c);
    free(result_i);
    fclose(fd);
}

int main(int argc, char const *argv[])
{
    check(argc != 2, "Uasge: ./tema3 <NR_PROBLEMA>");

    int task_nr = atoi(argv[1]);

    FILE *fd = fopen("tema3.in", "r");
    check(fd == NULL, "File in does not exits");

    Graph *g;

    if (task_nr == 1)
    {
        g = create_graph(fd, true);
        task1(g);
    }
    else if (task_nr == 2)
    {
        g = create_graph(fd, false);
        task2(g);
    }

    destory_graph(g);
    fclose(fd);
    return 0;
}
