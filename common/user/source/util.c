#define _XOPEN_SOURCE 700
#define __BSD_VISIBLE 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>

#include <ps4/util.h>

void ps4UtilStandardIORedirect(int to, int stdfd[3], fpos_t stdpos[3])
{
	int stdid[3] = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO};
	FILE *stdf[3] = {stdin, stdout, stderr};
	int i;

	for(i = 0; i < 3; ++i)
	{
		fflush(stdf[i]);

		if(stdpos != NULL)
			fgetpos(stdf[i], &stdpos[i]);

		if(stdfd != NULL)
			stdfd[i] = dup(stdid[i]);

		close(stdid[i]);
		dup(to);

		clearerr(stdf[i]);
	}
}

void ps4UtilStandardIORedirectReset(int stdfd[3], fpos_t stdpos[3])
{
	int stdid[3] = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO};
	FILE *stdf[3] = {stdin, stdout, stderr};
	int i;

	for(i = 0; i < 3; ++i)
	{
		fflush(stdf[i]);

		shutdown(stdid[i], SHUT_RDWR);
		close(stdid[i]);

		if(stdfd != NULL)
		{
			dup(stdfd[i]);
			close(stdfd[i]);
		}
		else //fill slots
			open("/dev/null", O_RDWR, 0);

		if(stdpos != NULL)
			fsetpos(stdf[i], &stdpos[i]);
		clearerr(stdf[i]);
	}
}

void ps4UtilStandardIoRedirectPlain(int to)
{
	int i;
	for(i = 0; i < 3; ++i)
	{
		close(i);
		if(to >= 0)
			dup(to);
		else
			open("/dev/null", O_RDWR, 0);
	}
}

int ps4UtilServerCreateEx(int port, int backlog, int try, unsigned int sec)
{
	int server;
	struct sockaddr_in serverAddress;
	int r;

	memset(&serverAddress, 0, sizeof(serverAddress));
	#ifdef __FreeBSD__ //parent of our __PS4__
	serverAddress.sin_len = sizeof(serverAddress);
	#endif
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	for(; try > 0; --try)
	{
		server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(server < 0)
			sleep(sec);
	}

	if(server < 0)
		return -1;

	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char *)&(int){ 1 }, sizeof(int));
	setsockopt(server, SOL_SOCKET, SO_REUSEPORT, (char *)&(int){ 1 }, sizeof(int));

	if((r = bind(server, (struct sockaddr *)&serverAddress, sizeof(serverAddress))) < 0)
	{
		close(server);
		return -2;
	}

	if((r = listen(server, backlog)) < 0)
	{
		close(server);
		return -3;
	}

	return server;
}

int ps4UtilServerCreateSingleAccept(int port)
{
	int server, client;
	if((server = ps4UtilServerCreateEx(port, 1, 20, 1)) < 0)
		return server;
	client = accept(server, NULL, NULL); // either return is fine
	close(server);
	return client;
}

FILE *ps4UtilFileOpenDescriptorDuplicate(int fd, const char *mode) //fddupopen
{
	int t;
	FILE *r = NULL;

	if(mode == NULL)
		return NULL;

	if((t = dup(fd)) < 0)
		return NULL;

	if((r = fdopen(t, mode)) == NULL)
		close(t);

	return r;
}

void *ps4UtilFileAllocateFromDescriptorUnsized(int fd, size_t *size)
{
	int length = 0;
	int full = 4096;
	uint8_t *data = (void *)malloc(full);
	size_t s = 0;

	if(size != NULL)
		*size = 0;

	while((length = read(fd, data + s, full - s)) > 0)
	{
		s += length;
		if(s == full)
		{
			void *t;
			full *= 2;
			t = malloc(full);
			memcpy(t, data, s);
			free(data);
			data = t;
		}
	}

	if(size != NULL)
		*size = s;

	return data;
}

void *ps4UtilFileAllocateFromPathAligned(char *file, size_t *size, size_t alignment)
{
	struct stat s;
	FILE *f;
	uint32_t *b;
	size_t sz;
	size_t i;

	if(size != NULL)
		*size = 0;

	if(stat(file, &s) < 0)
		return NULL;

	if(alignment == 0)
		alignment = 1;

 	sz = ((size_t)s.st_size * alignment) / alignment;
	b = (uint32_t *)malloc(sz * sizeof(uint8_t));

	if(b == NULL)
		return NULL;

	f = fopen(file, "rb");
	if(f == NULL)
	{
		free(b);
		return NULL;
	}
	fread(b, s.st_size, 1, f);
	fclose(f);

	if(size != NULL)
		*size = sz;

	for(i = s.st_size; i < sz; ++i)
		((uint32_t *)b)[i] = 0;

	return b;
}
