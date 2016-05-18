#define _KERNEL
#define _STANDALONE
#define __BSD_VISIBLE 1

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/libkern.h>
#include <sys/systm.h>
#include <sys/sysproto.h>

#include <sys/malloc.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/proc.h>
#include <sys/kthread.h>
#include <sys/ucred.h>
#include <sys/_iovec.h>
#include <sys/uio.h>
#include <sys/syscall.h>
#include <sys/sysent.h>
#include <sys/file.h>
#include <sys/capability.h>
#include <sys/fcntl.h>
#include <sys/filio.h>
#include <sys/filedesc.h>
#include <sys/syscallsubr.h>

#include <netinet/in.h>

#include <stdint.h>
#include <stdarg.h>

#include <ps4/kern.h>

int ps4KernUtilServerSocket(struct thread *td, int *sock, int domain, int type, int protocol)
{
	struct filedesc *fdp;
	struct socket *so;
	struct file *fp;
	int fd, error;

	if(sock == NULL)
		return EINVAL;

	fdp = td->td_proc->p_fd;
	error = falloc(td, &fp, &fd, 0);
	if (error)
		return (error);

	error = socreate(domain, &so, type, protocol, td->td_ucred, td);
	if (error) {
		fdclose(fdp, fp, fd, td);
	} else {
		finit(fp, FREAD | FWRITE, DTYPE_SOCKET, so, ps4KernDlSym("socketops"));
		*sock = fd;
	}
	fdrop(fp, td);

	return (error);
}

int ps4KernUtilSocketClose(struct thread *td, int sock)
{
	return kern_close(td, sock);
}

int ps4KernUtilSeverAccept(struct thread *td, int server, int *client, struct sockaddr **address, socklen_t *address_len)
{
	int r;

	if(client == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

 	r = kern_accept(td, server, address, address_len, NULL);
	if(r == 0)
		*client = ps4KernThreadGetReturn0(td);

	return r;
}

int ps4KernUtilServerListen(struct thread *td, int fd, int backlog)
{
	struct socket *so;
	struct file *fp;
	int error;

	fp = NULL;
	if ((td->td_proc->p_fd == NULL) || ((fp = fget_unlocked(td->td_proc->p_fd, fd)) == NULL))
		return (EBADF);

	if (fp->f_type != DTYPE_SOCKET) {
		fdrop(fp, td);
		return (ENOTSOCK);
	}

	so = fp->f_data;
	error = solisten(so, backlog, td);
	fdrop(fp, td);

	return(error);
}

int ps4KernUtilServerCreateEx(struct thread *td, int port, int backlog, int try, unsigned int sec)
{
	int server;
	struct sockaddr_in serverAddress;
	int r;

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_len = sizeof(serverAddress);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	for(; try > 0; --try)
	{
		server = -1;
		ps4KernUtilServerSocket(td, &server, PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(server >= 0)
			break;
		//	nanosleep(td, ...{1 << 9});
	}

	if(server < 0)
		return -1;

	r = kern_setsockopt(td, server, SOL_SOCKET, SO_REUSEADDR, (char *)&(int){ 1 }, UIO_SYSSPACE, sizeof(int));
	r = kern_setsockopt(td, server, SOL_SOCKET, SO_REUSEPORT, (char *)&(int){ 1 }, UIO_SYSSPACE, sizeof(int));

	r = kern_bind(td, server, (struct sockaddr *)&serverAddress);
	if(r != 0)
	{
		kern_close(td, server);
		return -2;
	}

	r = ps4KernUtilServerListen(td, server, backlog);
	if(r != 0)
	{
		kern_close(td, server);
		return -3;
	}

	return server;
}

int ps4KernUtilServerCreate(struct thread *td, int port)
{
	return ps4KernUtilServerCreateEx(td, port, 10, 20, 1);
}

int ps4KernUtilServerCreateSingleAccept(struct thread *td, int port)
{
	int server, client;
	if((server = ps4KernUtilServerCreateEx(td, port, 1, 20, 1)) < 0)
		return server;
	client = kern_accept(td, server, NULL, NULL, NULL);
	if(client == 0)
		client = ps4KernThreadGetReturn0(td);
	else
		client = -1;
	//ps4KernThreadSetReturn0(td, 0);
	kern_close(td, server);
	return client;
}

int ps4KernUtilSocketPrint(struct thread *td, int sock, const char *format, ...)
{
	va_list args;
	void *msg;
	size_t size;
	int r;

	if(td == NULL || format == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	va_start(args, format);
	r = ps4KernUtilStringAllocatePrintVariable((char **)&msg, (size_t *)&size, format, args);
	va_end(args);

	if(r != PS4_OK)
		return r;

	r = ps4KernUtilSocketSend(td, sock, msg, size);

	ps4KernMemoryFree(msg);

	return r;
}

int ps4KernUtilSocketSend(struct thread *td, int sock, const void *data, size_t size)
{
	struct msghdr msg;
	struct iovec aiov;
	int r;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &aiov;
	msg.msg_iovlen = 1;
	msg.msg_control = 0;
	aiov.iov_base = (void *)data;
	aiov.iov_len = size;

	if(td == NULL || data == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	if(size == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

	r = kern_sendit(td, sock, &msg, 0, NULL, UIO_SYSSPACE);

	return r;
}

int ps4KernUtilStringAllocatePrint(char **string, size_t *size, const char *format, ...)
{
	va_list args;
	int r;

	if(string == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(size == NULL || format == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	va_start(args, format);
	r = ps4KernUtilStringAllocatePrintVariable(string, size, format, args);
	va_end(args);

	return r;
}

int ps4KernUtilStringAllocatePrintVariable(char **string, size_t *size, const char *format, va_list args)
{
	va_list vargs;
	size_t sz;
	char *s;

	if(string == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(size == NULL || format == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	va_copy(vargs, args);
	sz = vsnprintf(NULL, 0, format, args);
	s = NULL;
	if(sz > 0)
	{
		s = ps4KernMemoryMalloc(sz + 2);
		vsnprintf(s, sz + 1, format, vargs);
	}
	va_end(vargs);

	if(size != NULL)
		*size = sz;
	*string = s;

	return PS4_OK;
}

int ps4KernUtilFileWrite(struct thread *td, int fd, const void *data, size_t size)
{
	struct uio auio;
	struct iovec aiov;
	int r;

	if(size == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

	aiov.iov_base = (void *)data;
	aiov.iov_len = size;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_resid = size;
	auio.uio_segflg = UIO_SYSSPACE;

	r = kern_writev(td, fd, &auio);

	return r;
}

int ps4KernUtilFilePrint(struct thread *td, int fd, const char *format, ...)
{
	va_list args;
	void *msg;
	size_t size;
	int r;

	if(td == NULL || format == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	va_start(args, format);
	r = ps4KernUtilStringAllocatePrintVariable((char **)&msg, (size_t *)&size, format, args);
	va_end(args);

	if(r != PS4_OK)
		return r;

	r = ps4KernUtilFileWrite(td, fd, msg, size);

	ps4KernMemoryFree(msg);

	return r;
}
