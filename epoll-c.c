#define MAX_EVENTS 24

static int _epoll_create(void) {
    /* Returns an epfd */
    return epoll_create(MAX_EVENTS);
}

static int _epoll_ctl(int epfd, int op, int fd, int events) {
    int res;
    struct epoll_event event;

    memset(&event, 0, sizeof(event));
    event.events = events;
    event.data.fd = fd;

    if (-1 == (res = epoll_ctl(epfd, op, fd, &event))) {
        perror("epoll_ctl");
    }

    return res;
}

static C_word _epoll_wait(int epfd, int timeout) {
    /* Wrapper around epoll_wait. It calls a scheme callback
     * function with a vector of pairs */
    struct epoll_event events[MAX_EVENTS];
    int num_events, i;
    C_word *vecp, *v0;
    C_word *pairp;

    num_events = epoll_wait(epfd, events, MAX_EVENTS, timeout);

    vecp = C_alloc(C_SIZEOF_VECTOR(num_events));
    pairp = C_alloc(C_SIZEOF_PAIR * num_events);

    v0 = vecp;
    *(vecp++) = num_events;

    for (i = 0; i < num_events; ++i) {
        *(vecp++) = C_pair(&pairp, C_fix(events[i].data.fd), C_fix(events[i].events));
    }

    return (C_word)v0;
}
