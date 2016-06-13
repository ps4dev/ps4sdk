#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>

#include <sys/file.h>
#include <sys/fcntl.h>
#include <sys/filio.h>
#include <sys/filedesc.h>
#include <sys/syscallsubr.h>

#include <ps4/kernel.h>

int ps4KernelDescriptorWrite(struct thread *td, int fd, const void *data, size_t size)
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

int ps4KernelDescriptorPrint(struct thread *td, int fd, const char *format, ...)
{
	va_list args;
	void *msg;
	size_t size;
	int r;

	if(td == NULL || format == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	va_start(args, format);
	//FIXME: loop-write, limit size
	r = ps4KernelMemoryAllocateStringWithArgumentList((char **)&msg, (size_t *)&size, 0, format, args);
	va_end(args);

	if(r != PS4_OK)
		return r;

	r = ps4KernelDescriptorWrite(td, fd, msg, size);

	ps4KernelMemoryFree(msg);

	return r;
}
