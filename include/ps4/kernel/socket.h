#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

typedef struct socket Ps4KernelSocket;

int ps4KernelSocketCreate(struct thread *td, Ps4KernelSocket **sock, int domain, int type, int protocol);
int ps4KernelSocketTCPCreate(struct thread *td, Ps4KernelSocket **sock);
int ps4KernelSocketTCPServerCreate(struct thread *td, Ps4KernelSocket **oserver, int port, int backlog);
int ps4KernelSocketTCPServerCreateAcceptThenDestroy(struct thread *td, Ps4KernelSocket **client, int port);
int ps4KernelSocketClose(Ps4KernelSocket *sock);
int ps4KernelSocketDestroy(Ps4KernelSocket *sock);

int ps4KernelSocketBind(struct thread *td, Ps4KernelSocket *sock, struct sockaddr *address);
int ps4KernelSocketAddressGet(Ps4KernelSocket *sock, struct sockaddr **address, socklen_t *address_len);
int ps4KernelSocketOptionSet(Ps4KernelSocket *sock, int level, int name, void *value, socklen_t valueSize);
int ps4KernelSocketAccept(Ps4KernelSocket *server, Ps4KernelSocket **client);
int ps4KernelSocketListen(struct thread *td, Ps4KernelSocket *fd, int backlog);

int ps4KernelSocketPrintSizedWithArgumentList(struct thread *td, Ps4KernelSocket *sock, size_t sz, const char *format, va_list args);
int ps4KernelSocketPrintWithArgumentList(struct thread *td, Ps4KernelSocket *sock, const char *format, va_list args);
int ps4KernelSocketPrint(struct thread *td, Ps4KernelSocket *sock, const char *format, ...);
int ps4KernelSocketPrintHexDump(struct thread *td, Ps4KernelSocket *sock, const void *data, size_t size);
int ps4KernelSocketSend(struct thread *td, Ps4KernelSocket *sock, const void *data, size_t size);

int ps4KernelSocketReceiveString(struct thread *td, Ps4KernelSocket *sock, void **data, size_t *size, size_t sizeMax);

int ps4KernelSocketScanSizedWithArgumentList(struct thread *td, Ps4KernelSocket *sock, size_t size, int *match, const char *format, va_list args);
int ps4KernelSocketScanWithArgumentList(struct thread *td, Ps4KernelSocket *sock, int *match, const char *format, va_list args);
int ps4KernelSocketScan(struct thread *td, Ps4KernelSocket *sock, int *match, const char *format, ...);
int ps4KernelSocketReceive(Ps4KernelThread *td, Ps4KernelSocket *sock, const void *data, size_t *size, size_t sizeMax, int flags);
