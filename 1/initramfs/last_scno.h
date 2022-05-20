// last_scno.h: header contendo a syscall (usado pelo init.c)

#include <unistd.h>
#include <sys/syscall.h>
#define __GNU_SOURCE
#define __NR_LAST_SCNO 404

long last_scno()
{
    return syscall(__NR_LAST_SCNO);
}