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
