#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include <ps4/util.h>
#include <ps4/error.h>

void ps4StandardIoRedirect(int to)
{
	int i;
	for(i = 0; i < 3; ++i)
	{
		if(i == to)
			continue;

		close(i);
		if(to > 0)
			dup(to);
		else
			open("/dev/null", O_RDWR, 0);
	}
}

int ps4StandardIoPrintHexDump(const void *data, size_t size)
{
	uint8_t *d = (uint8_t *)data;
	size_t consoleSize = 16;
	uint8_t b[consoleSize + 3];
	size_t i;

	if(data == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	b[0] = '|';
	b[consoleSize + 1] = '|';
	b[consoleSize + 2] = '\0';
	for (i = 0; i < size; i++)
	{
		if ((i % consoleSize) == 0)
		{
			if (i != 0)
				printf("  %s\n", b);
			printf("%016lx ", (uint8_t *)data + i);
		}

		if(i % consoleSize == 8)
			printf(" ");
		printf(" %02x", d[i]);

		if (d[i] >= ' ' && d[i] <= '~')
			b[i % consoleSize + 1] = d[i];
		else
			b[i % consoleSize + 1] = '.';
	}

	while((i % consoleSize) != 0)
	{
		if(i % consoleSize == 8)
			printf("    ");
		else
			printf("   ");
		b[i % consoleSize + 1] = '.';
		i++;
	}

	printf("  %s\n", b);

	return PS4_OK;
}
