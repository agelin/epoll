#include <sys/epoll.h>
#include <fcntl.h>
#include <string.h>

static int _epoll_create(void);
static int _epoll_ctl(int, int, int, int);
static C_word _epoll_wait(int, int);
