#include <time.h>
#include <errno.h>
#include <internal/syscall.h>

int nanosleep(const struct timespec *req, struct timespec *rem) {
    int result = syscall(__NR_nanosleep, req, rem);

    if (result < 0)
    {
        errno = -result;
        return -1;
    }

    return result;
}
