#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/libkern.h>

#include <sys/malloc.h>

#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/proc.h>
#include <sys/kthread.h>
#include <sys/ucred.h>

#include <sys/_iovec.h>
#include <sys/uio.h>

#include <sys/syscallsubr.h>

#include <sys/mutex.h>
#include <net/vnet.h>
#include <sys/protosw.h>

#include <ps4/kernel.h>
#include <ps4/error.h>

int ps4KernelSocketCreate(struct thread *td, Ps4KernelSocket **sock, int domain, int type, int protocol)
{
	if(td == NULL)
		return PS4_ERROR_ARGUMENT_THREAD_MISSING;
	if(sock == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	return socreate(domain, sock, type, protocol, td->td_proc->p_ucred, td);
}

int ps4KernelSocketTCPCreate(struct thread *td, Ps4KernelSocket **sock)
{
	return ps4KernelSocketCreate(td, sock, PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int ps4KernelSocketTCPServerCreate(struct thread *td, Ps4KernelSocket **oserver, int port, int backlog)
{
	struct socket *server;
	struct sockaddr_in address;
	int r;

	if(td == NULL)
		return PS4_ERROR_ARGUMENT_THREAD_MISSING;
	if(oserver == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	ps4KernelMemoryFill(&address, 0, sizeof(address));
	address.sin_len = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	r = ps4KernelSocketTCPCreate(td, &server);
	if(r != PS4_OK)
		return r;

	r = ps4KernelSocketOptionSet(server, SOL_SOCKET, SO_REUSEADDR, (char *)&(int){ 1 }, sizeof(int));
	r = ps4KernelSocketOptionSet(server, SOL_SOCKET, SO_REUSEPORT, (char *)&(int){ 1 }, sizeof(int));

	r = ps4KernelSocketBind(td, server, (struct sockaddr *)&address);
	if(r != PS4_OK)
	{
		ps4KernelSocketDestroy(server);
		return r;
	}

	r = ps4KernelSocketListen(td, server, backlog);
	if(r != PS4_OK)
	{
		ps4KernelSocketDestroy(server);
		return r;
	}

	*oserver = server;

	return PS4_OK;
}

int ps4KernelSocketTCPServerCreateAcceptThenDestroy(struct thread *td, struct socket **client, int port)
{
	struct socket *server;
	int r;

	if(td == NULL)
		return PS4_ERROR_ARGUMENT_THREAD_MISSING;
	if(client == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	r = ps4KernelSocketTCPServerCreate(td, &server, port, 1);
	if(r != PS4_OK)
		return r;
	r = ps4KernelSocketAccept(server, client);
	ps4KernelSocketDestroy(server);

	return r;
}

int ps4KernelSocketClose(Ps4KernelSocket *sock)
{
	return ps4KernelSocketDestroy(sock);
}

int ps4KernelSocketDestroy(Ps4KernelSocket *sock)
{
	if(sock == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	return soclose(sock);
}

int ps4KernelSocketBind(struct thread *td, Ps4KernelSocket *sock, struct sockaddr *address)
{
	if(td == NULL)
		return PS4_ERROR_ARGUMENT_THREAD_MISSING;
	if(sock == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	return sobind(sock, address, td);
}

int ps4KernelSocketListen(struct thread *td, Ps4KernelSocket *sock, int backlog)
{
	if(td == NULL)
		return PS4_ERROR_ARGUMENT_THREAD_MISSING;
	if(sock == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	return solisten(sock, backlog, td);
}

int ps4KernelSocketAddressGet(Ps4KernelSocket *sock, struct sockaddr **address, socklen_t *address_len)
{
	if(sock == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(address == NULL || address_len == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	if((sock->so_state & (SS_ISCONNECTED|SS_ISCONFIRMING)) == 0)
		return ENOTCONN;

	*address = NULL;
	CURVNET_SET(sock->so_vnet);
	(*sock->so_proto->pr_usrreqs->pru_peeraddr)(sock, address);
	CURVNET_RESTORE();

	if(*address != NULL)
		*address_len = (*address)->sa_len;

	return PS4_OK;
}

int ps4KernelSocketOptionSet(Ps4KernelSocket *sock, int level, int name, void *value, socklen_t valueSize)
{
	struct sockopt sopt;

	if(sock == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	sopt.sopt_dir = SOPT_SET;
	sopt.sopt_level = level;
	sopt.sopt_name = name;
	sopt.sopt_val = value;
	sopt.sopt_valsize = valueSize;
	sopt.sopt_td = NULL;

	return sosetopt(sock, &sopt);
}

int ps4KernelSocketAccept(Ps4KernelSocket *head, struct socket **client)
{
	struct mtx *accept_mtx;
	struct socket *so;
	struct sockaddr *sa;
	pid_t pgid;
	int r = 0;

	if(head == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(client == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	head->so_options |= SO_ACCEPTCONN;
	if((head->so_options & SO_ACCEPTCONN) == 0)
		return EINVAL;

	ps4ExpressionReturnOnError(ps4KernelSymbolLookUp("accept_mtx", (void **)&accept_mtx));

	mtx_lock(accept_mtx);

	if((head->so_state & SS_NBIO) && TAILQ_EMPTY(&head->so_comp))
	{
		mtx_unlock(accept_mtx);
		return EWOULDBLOCK;
	}

	while(TAILQ_EMPTY(&head->so_comp) && head->so_error == 0)
	{
		if(head->so_rcv.sb_state & SBS_CANTRCVMORE)
		{
			mtx_unlock(accept_mtx);
			return ECONNABORTED;
		}

		r = msleep(&head->so_timeo, accept_mtx, PSOCK | PCATCH, "accept", 0);
		if(r)
		{
			mtx_unlock(accept_mtx);
			return r;
		}
	}

	so = TAILQ_FIRST(&head->so_comp);

	SOCK_LOCK(so);
	soref(so);

	TAILQ_REMOVE(&head->so_comp, so, so_list);
	head->so_qlen--;
	so->so_state |= (head->so_state & SS_NBIO);
	so->so_qstate &= ~SQ_COMP;
	so->so_head = NULL;

	SOCK_UNLOCK(so);

	mtx_unlock(accept_mtx);

	KNOTE_UNLOCKED(&head->so_rcv.sb_sel.si_note, 0);

	pgid = fgetown(&head->so_sigio);
	if (pgid != 0)
		fsetown(pgid, &so->so_sigio);

	sa = NULL;
	r = soaccept(so, &sa);
	if(sa)
	{
		static struct malloc_type *mt;
		ps4ExpressionReturnOnError(ps4KernelSymbolLookUp("M_SONAME", (void **)&mt));
		free(sa, mt);
	}

	*client = so;

	return r;
}

int ps4KernelSocketPrintSizedWithArgumentList(struct thread *td, Ps4KernelSocket *sock, size_t sz, const char *format, va_list args)
{
	void *msg;
	size_t size;
	int r;

	if(td == NULL)
		return PS4_ERROR_ARGUMENT_THREAD_MISSING;
	if(format == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	r = ps4KernelMemoryAllocateStringWithArgumentList((char **)&msg, (size_t *)&size, sz, format, args);

	if(r != PS4_OK)
		return r;

	r = ps4KernelSocketSend(td, sock, msg, size);

	ps4KernelMemoryFree(msg);

	return r;
}

int ps4KernelSocketPrintWithArgumentList(struct thread *td, Ps4KernelSocket *sock, const char *format, va_list args)
{
	return ps4KernelSocketPrintSizedWithArgumentList(td, sock, 0, format, args);
}

int ps4KernelSocketPrint(struct thread *td, Ps4KernelSocket *sock, const char *format, ...)
{
	va_list args;
	int r;

	va_start(args, format);
	r = ps4KernelSocketPrintWithArgumentList(td, sock, format, args);
	va_end(args);

	return r;
}

int ps4KernelSocketPrintHexDump(struct thread *td, Ps4KernelSocket *sock, const void *data, size_t size)
{
	uint8_t *d = (uint8_t *)data;
	size_t consoleSize = 16;
	uint8_t b[consoleSize + 3];
	size_t i;

	if(td == NULL)
		return PS4_ERROR_ARGUMENT_THREAD_MISSING;
	if(sock == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(data == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	b[0] = '|';
	b[consoleSize + 1] = '|';
	b[consoleSize + 2] = '\0';
	for (i = 0; i < size; i++)
	{
		if ((i % consoleSize) == 0)
		{
			if (i != 0)
				ps4KernelSocketPrint(td, sock, "  %s\n", b);
			ps4KernelSocketPrint(td, sock, "%016lx ", (uint8_t *)data + i);
		}

		if(i % consoleSize == 8)
			ps4KernelSocketPrint(td, sock, " ");
		ps4KernelSocketPrint(td, sock, " %02x", d[i]);

		if (d[i] >= ' ' && d[i] <= '~')
			b[i % consoleSize + 1] = d[i];
		else
			b[i % consoleSize + 1] = '.';
	}

	while((i % consoleSize) != 0)
	{
		if(i % consoleSize == 8)
			ps4KernelSocketPrint(td, sock, "    ");
		else
			ps4KernelSocketPrint(td, sock, "   ");
		b[i % consoleSize + 1] = '.';
		i++;
	}

	ps4KernelSocketPrint(td, sock, "  %s\n", b);

	return PS4_OK;
}

int ps4KernelSocketSend(struct thread *td, Ps4KernelSocket *sock, const void *data, size_t size)
{
	struct uio auio;
	struct iovec iov;

	if(td == NULL)
		return PS4_ERROR_ARGUMENT_THREAD_MISSING;
	if(sock == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(size == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

	iov.iov_base = (void *)data;
	iov.iov_len = size;

	auio.uio_iov = &iov;
	auio.uio_iovcnt = 1;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_rw = UIO_WRITE;
	auio.uio_td = td;
	auio.uio_offset = 0;
	//auio.uio_resid = 0;
	auio.uio_resid = iov.iov_len;

	return sosend(sock, NULL, &auio, 0, NULL, 0, td);
}

int ps4StringEstimateFormatArguments(const char *format, size_t *count)
{
	size_t c = 0;

	if(format == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(count == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	for(size_t i = 0; format[i] != '\0'; ++i)
	{
		if(format[i] == '%')
		{
			++c;

			if(format[i + 1] == '\0')
				return PS4_ERROR_ARGUMENT_MALFORMED;

			if(format[i + 1] == '%')
				i++;
		}
	}

	*count = c;

	return PS4_OK;
}

// FIXME: Slow, improve me
int ps4KernelSocketReceiveString(struct thread *td, Ps4KernelSocket *sock, void **data, size_t *size, size_t sizeMax)
{
	uint8_t *_data;
	size_t _size;
	size_t offset;
	size_t outSize;
	int r;

	if(sizeMax == 0)
		sizeMax = SIZE_MAX;

	_size = 1024;
	if(_size > sizeMax - 1)
		_size = sizeMax - 1;

	offset = 0;

	ps4KernelMemoryAllocateData((void **)&_data, _size + 1);

	while(1)
	{
		r = ps4KernelSocketReceive(td, sock, _data + offset, &outSize, 1, MSG_WAITALL);

		if(r != PS4_OK)
		{
			ps4KernelMemoryFree(data);
			return PS4_ERROR_BAD_REQUEST;
		}

		if(_data[offset] == '\n')
			break;

		if(offset == _size)
		{
			_size *= 2;
			ps4KernelMemoryReallocateData((void **)&_data, _size + 1);
		}

		++offset;
	}

	_data[offset + 1] = '\0';
	*data = (void *)_data;
	*size = offset + 2;

	return PS4_OK;
}

int ps4KernelSocketScanSizedWithArgumentList(struct thread *td, Ps4KernelSocket *sock, size_t size, int *match, const char *format, va_list args)
{
	void *data;
	int r;
	size_t outSize;

	if(match == NULL || format == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	r = ps4KernelSocketReceiveString(td, sock, &data, &outSize, size);

	if(r == PS4_OK)
	{
		*match = vsscanf(data, format, args);
		ps4KernelMemoryFree(data);
	}

	return r;
}

int ps4KernelSocketScanWithArgumentList(struct thread *td, Ps4KernelSocket *sock, int *match, const char *format, va_list args)
{
	return ps4KernelSocketScanSizedWithArgumentList(td, sock, 0, match, format, args);
}

int ps4KernelSocketScan(struct thread *td, Ps4KernelSocket *sock, int *match, const char *format, ...)
{
	va_list args;
	int r;

	va_start(args, format);
	r = ps4KernelSocketScanWithArgumentList(td, sock, match, format, args);
	va_end(args);

	return r;
}

int ps4KernelSocketReceive(Ps4KernelThread *td, Ps4KernelSocket *sock, const void *data, size_t *size, size_t sizeMax, int flags)
{
	struct uio auio;
	struct iovec iov;

	if(td == NULL)
		return PS4_ERROR_ARGUMENT_THREAD_MISSING;
	if(sock == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(data == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	if(data == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	if(sizeMax == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

	iov.iov_base = (void *)data;
	iov.iov_len = sizeMax;

	auio.uio_iov = &iov;
	auio.uio_iovcnt = 1;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_td = td;
	auio.uio_offset = 0;
	auio.uio_resid = iov.iov_len;

	int r = soreceive(sock, NULL, &auio, NULL, NULL, &flags);

	*size = sizeMax - iov.iov_len;

	return r;
}
