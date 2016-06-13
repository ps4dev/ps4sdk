#pragma once

int ps4SocketTCPServerCreate(int *server, int port, int backlog);
int ps4SocketTCPServerCreateAcceptThenDestroy(int *client, int port);
