#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <ps4/util.h>
#include <ps4/error.h>

int ps4SocketTCPServerCreate(int *oserver, int port, int backlog)
{
	int server;
	struct sockaddr_in address;
	int r;

	if(oserver == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	memset(&address, 0, sizeof(address));
	address.sin_len = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server < 0)
		return -1;

	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char *)&(int){ 1 }, sizeof(int));
	setsockopt(server, SOL_SOCKET, SO_REUSEPORT, (char *)&(int){ 1 }, sizeof(int));

	if((r = bind(server, (struct sockaddr *)&address, sizeof(address))) < 0)
	{
		close(server);
		return -2;
	}

	if((r = listen(server, backlog)) < 0)
	{
		close(server);
		return -3;
	}

	*oserver = server;

	return PS4_OK;
}

int ps4SocketTCPServerCreateAcceptThenDestroy(int *oclient, int port)
{
	int server, client, r;

	if(oclient == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	r = ps4SocketTCPServerCreate(&server, port, 1);
	if(r != PS4_OK)
		return r;
	client = accept(server, NULL, NULL);
	close(server);

	*oclient = client;

	if(client < 0)
		return client;
	return PS4_OK;
}
