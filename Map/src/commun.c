#include <stdlib.h>
#include <stdio.h>
#include "../header.h"

void check(int expr, char *message)
{
    if (expr) {
        printf("%s\n", message);
        exit(EXIT_FAILURE);
    }
}

int compare_int(const void *a, const void *b)
{
    int int_a = *((int *)a);
    int int_b = *((int *)b);

    if (int_a < int_b)
        return -1;
    else if (int_a > int_b)
        return 1;
    else
        return 0;
}
