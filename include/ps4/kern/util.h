#pragma once

#include <sys/socket.h>

#include <stdarg.h>

int ps4KernUtilServerSocket(struct thread *td, int *sock, int domain, int type, int protocol);
int ps4KernUtilSocketClose(struct thread *td, int sock);
int ps4KernUtilSeverAccept(struct thread *td, int server, int *client, struct sockaddr **address, socklen_t *address_len);
int ps4KernUtilServerBind(struct thread *td, int fd, void *value, int length);
int ps4KernUtilServerListen(struct thread *td, int fd, int backlog);
int ps4KernUtilServerCreateEx(struct thread *td, int port, int backlog, int try, unsigned int sec);
int ps4KernUtilServerCreate(struct thread *td, int port);
int ps4KernUtilServerCreateSingleAccept(struct thread *td, int port);
int ps4KernUtilSocketSend(struct thread *td, int sock, const void *msg, size_t size);
int ps4KernUtilSocketPrint(struct thread *td, int sock, const char *format, ...);
int ps4KernUtilStringAllocatePrint(char **string, size_t *size, const char *format, ...);
int ps4KernUtilStringAllocatePrintVariable(char **string, size_t *size, const char *format, va_list args);
int ps4KernUtilFileWrite(struct thread *td, int fd, const void *data, size_t size);
int ps4KernUtilFilePrint(struct thread *td, int fd, const char *format, ...);
