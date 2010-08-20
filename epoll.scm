;;;; epoll.scm
;
; Copyright (c) 2010, David Reynolds
; All rights reserved.

; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are
; met:
;
;    * Redistributions of source code must retain the above copyright
;      notice, this list of conditions and the following disclaimer.
;
;    * Redistributions in binary form must reproduce the above
;      copyright notice, this list of conditions and the following disclaimer
;      in the documentation and/or other materials provided with the
;      distribution.
;
;    * Neither the name of the author nor the names of its
;      contributors may be used to endorse or promote products derived from
;      this software without specific prior written permission.

; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
; "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
; LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
; A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
; OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
; SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
; LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
; DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
; THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
; Send bugs, suggestions, and ideas to:
;
; david@alwaysmovefast.com

(module epoll
    (epoll-create
     epoll-ctl
     epoll-wait
     epoll-add
     epoll-modify
     epoll-delete
     _READ _WRITE _ERROR)

    (import scheme chicken foreign)
    (use tcp)

(foreign-declare #<<EOF
#include "epoll-c.h"
#include "epoll-c.c"
EOF
)

(define-foreign-variable EPOLLIN int)
(define-foreign-variable EPOLLPRI int)
(define-foreign-variable EPOLLOUT int)
(define-foreign-variable EPOLLERR int)
(define-foreign-variable EPOLLHUP int)
(define-foreign-variable EPOLLRDHUP int)
(define-foreign-variable EPOLLONESHOT int)
(define-foreign-variable EPOLLET int)

(define _READ EPOLLIN)
(define _WRITE EPOLLOUT)
(define _ERROR (bitwise-ior EPOLLERR EPOLLHUP EPOLLRDHUP))

(define EPOLL_CTL_ADD 1)
(define EPOLL_CTL_DEL 2)
(define EPOLL_CTL_MOD 3)

(define epoll-create (foreign-lambda int "_epoll_create"))
(define epoll-ctl (foreign-lambda int "_epoll_ctl" int int int int))

(define user-defined-callback #f)

(define _epoll_wait (foreign-safe-lambda void "_epoll_wait" int int))

;; wrapper for _epoll_wait that accepts a callback into the user's application and sets it globally
(define (epoll-wait epfd timeout user-callback)
    (if (eq? user-defined-callback #f)
        (set! user-defined-callback user-callback))
    (_epoll_wait epfd timeout))

(define-external (SCM_epoll_wait_cb (scheme-object vec)) void
    (let ((li (vector->list vec)))
        ;; call the callback that was supplied by the user in epoll-wait
        (user-defined-callback li)))

(define (epoll-add epfd fd iostate)
    (epoll-ctl epfd EPOLL_CTL_ADD fd iostate))

(define (epoll-modify epfd fd iostate)
    (epoll-ctl epfd EPOLL_CTL_MOD fd iostate))

(define (epoll-delete epfd fd)
    (epoll-ctl epfd EPOLL_CTL_DEL fd 0))

)
