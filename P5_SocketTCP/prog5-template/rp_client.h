#define RP_REMOTE 0
#define RP_LOCAL  1
#define RP_MIXED  2


#if RP_TYPE==RP_REMOTE

#define open    rp_open
#define close   rp_close
#define read    rp_read
#define write   rp_write
#define lseek   rp_lseek

#include "rp_procs.h"

#elif RP_TYPE==RP_LOCAL

#define rp_connect      rp_connect_dummy
#define rp_checksum     rp_checksum_dummy

int rp_connect_dummy(__attribute__((unused)) const char *address, __attribute__((unused)) unsigned short port) { return 0; }
short rp_checksum_dummy(__attribute__((unused)) int fd) { return 0; }

#elif RP_TYPE==RP_MIXED

#include "rp_procs.h"

#endif


