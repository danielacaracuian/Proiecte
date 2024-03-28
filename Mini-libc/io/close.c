// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>

int close(int fd)
{
    /* TODO: Implement close(). */
    int result = syscall(__NR_close, fd);

    if (result == -1)
    {
        errno = EBADF;
        return -1;
    }

    return 0;
}
