#include <stddef.h>
#include <sys/types.h>

int rp_open(const char* pathname, int flags, ...);
int rp_close(int fd);
ssize_t rp_read(int fd, void *buf, size_t count);
ssize_t rp_write(int fd, const void *buf, size_t count);
off_t rp_lseek(int fd, off_t offset, int whence);

int rp_connect(const char *address, unsigned short port);
short rp_checksum(int fd);
