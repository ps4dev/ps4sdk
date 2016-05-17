#pragma once

//int ps4KernUtilServerCreate(struct thread *td, struct socket **oserver, int port, int backlog);

//int ps4KernUtilServerCreateEx(struct thread *td, int port, int backlog, int try, unsigned int sec);
//#define ps4KernUtilServerCreate(td, port) ps4KernUtilServerCreateEx(td, port, 10, 1, 0)

int ps4KernUtilServerSocket(struct thread *td, int *sock, int domain, int type, int protocol);
int ps4KernUtilServerBind(struct thread *td, int fd, void *value, int length);
int ps4KernUtilServerListen(struct thread *td, int fd, int backlog);
int ps4KernUtilServerCreateEx(struct thread *td, int port, int backlog, int try, unsigned int sec);
int ps4KernUtilServerCreate(struct thread *td, int port);
int ps4KernUtilServerCreateSingleAccept(struct thread *td, int port);
