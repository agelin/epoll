/*
 * Copyright (c) 2010, David Reynolds
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following disclaimer
 *       in the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the author nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
