#include <unistd.h>

int puts(const char *s) {
    int result = 0;

    while (*s != '\0') {
        result = write(1, s, 1);
        if (result < 0) {
            return -1;
        }
        s++;
    }

    result = write(1, "\n", 1);
    if (result < 0) {
        return -1;
    }

    return 0;
}
