// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <errno.h>

off_t lseek(int fd, off_t offset, int whence)
{
    off_t new_offset;

    new_offset = syscall(__NR_lseek, fd, offset, whence);

    if (new_offset == -1)
    {
        errno = -new_offset;
        return -1;
    }
    if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END)
    {
        errno = EINVAL;
        return -1;
    }
    if (offset < 0)
    {
        errno = EINVAL;
        return -1;
    }

    if (fd < 0)
    {
        errno = EBADF;
        return -1;
    }

    if (new_offset == -1)
    {
        errno = -new_offset;
        return -1;
    }

    return new_offset;
}
