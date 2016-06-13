#include <unistd.h>

#include <ps4/util.h>
#include <ps4/error.h>

int ps4StreamOpenFileDuplicate(FILE **file, int fd, const char *mode) //fddupopen
{
	int t;
	FILE *r = NULL;

	if(file == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(mode == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	if((t = dup(fd)) < 0)
		return PS4_ERROR_INTERNAL_ERROR;

	if((r = fdopen(t, mode)) == NULL)
		close(t);

	*file = r;

	return PS4_OK;
}
