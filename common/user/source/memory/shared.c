#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <ps4/memory/shared.h>

typedef struct Ps4MemoryShared
{
	void *address;
	size_t size;
	char *path;
}
Ps4MemoryShared;

// This allows for stable webbrowser reopen and ipc m

//FIXME: Implement Refcount and/or seperate unlink/close ?
int ps4MemorySharedOpen(Ps4MemoryShared **memory, const char *path, size_t size)
{
	int handle;
	Ps4MemoryShared *m;
	struct stat st;
	size_t fileSize;
	long pageSize = sysconf(_SC_PAGESIZE);
	size_t l = strnlen(path, 255);

	if(memory == NULL)
		return -1;

	m = (Ps4MemoryShared *)malloc(sizeof(Ps4MemoryShared));
	if(m == NULL)
		return -2;

	m->path = malloc((l + 1)* sizeof(char));
	if(m->path == NULL)
		goto e1;

	strncpy(m->path, path, l);
	m->path[l] = '\0';

	handle = shm_open(path, O_CREAT | O_RDWR, 0755);

	if(handle < 0)
		goto e2;

	fileSize = 0;
	if(fstat(handle, &st) == 0)
		fileSize = (size_t)st.st_size;

	if(size == 0 && fileSize > 0)
		m->size = ((fileSize - 1) / pageSize + 1) * pageSize;
	else if(size == 0)
		m->size = pageSize;
	else
		m->size = ((size - 1) / pageSize + 1) * pageSize;

	if(m->size > fileSize)
		if(ftruncate(handle, m->size) < 0)
			goto e3;

	m->address = mmap(NULL, m->size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, handle, 0);
	if(m->address == MAP_FAILED)
		goto e3;

	close(handle);

	*memory = m;
	return 0;

	e3:
		close(handle);
		shm_unlink(m->path);
	e2:
		free(m->path);
	e1:
		free(m);

	return -3;
}

int ps4MemorySharedClose(Ps4MemoryShared *m)
{
	int r = 0;
	if(m == NULL)
		return -1;
	//r |= shm_unlink(m->path);
	r |= munmap(m->address, m->size);
	free(m->path);
	free(m);
	return r;
}

int ps4MemorySharedUnlink(Ps4MemoryShared *m)
{
	int r = 0;
	if(m == NULL)
		return -1;
	r |= shm_unlink(m->path);
	r |= ps4MemorySharedClose(m);
	return r;
}

void *ps4MemorySharedGetAddress(Ps4MemoryShared *m)
{
	if(m == NULL)
		return NULL;
	return m->address;
}

size_t ps4MemorySharedGetSize(Ps4MemoryShared *m)
{
	if(m == NULL)
		return 0;
	return m->size;
}
