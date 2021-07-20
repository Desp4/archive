#ifndef KENT_NETW_H
#define KENT_NETW_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32

#include <WS2tcpip.h> // link to Ws2_32

typedef SOCKET Socket;

#else // _WIN32

#include <netinet/in.h>
#include <errno.h>

#define INVALID_SOCKET -1

typedef int Socket;

#endif // UNIX

#define IPV4_ADDRLEN 16

typedef enum
{
    TCP = 1,
    UDP = 2
} Protocol;

/*
Write public IPv4 of the caller to buf.

buf - output buffer, should be at lest IPV4_ADDRLEN in length
len - buf length, bytes written will be written here

Returns 0 on success, otherwise system error code on Windows
or curl error code on Linux.
*/
int netw_public_ip_v4(char *buf, int *len);

/*
Initialize networking.
On Linux initializes libcurl.

Returns 0 on success, otherwise system error code on Windows
or curl error code on Linux.
*/
int netw_init();

/*
Terminate networking.
On Linux terminates libcurl.

Returns 0 on success, otherwise system error code on Windows
or curl error code on Linux.
*/
int netw_terminate();

/*
Close socket.

Returns 0 on success, otherwise WSA error on Windows
or system error on Linux.
*/
int netw_socket_close(Socket sock);

/*
Open a client socket and connect to an ip.

ip     - server ip, null terminated
port   - server port
proto  - connection protocol
client - output client socket

Returns 0 on success, otherwise WSA error on Windows
or system error on Linux.
*/
int netw_client_create(const char *ip, unsigned short port, Protocol proto, Socket *client);

/*
Create either a UDP or TCP server.

Returns 0 on success, otherwise WSA error on Windows
or system error on Linux.
*/
int netw_server_create(Socket *serv, Protocol proto);

/*
Bind server to a port.

Returns 0 on success, otherwise WSA error on Windows
or system error on Linux.
*/
int netw_server_bind(Socket *serv, unsigned short port);

/*
Set server to listen for connections.

queue - maximum pending connections.

Returns 0 on success, otherwise WSA error on Windows
or system error on Linux.
*/
int netw_server_listen(Socket *serv, int queue);

/*
Wait for a connection.

serv   - server socket
out    - output for a connected socket
client - output for connected socket info

Returns 0 on success, otherwise WSA error on Windows
or system error on Linux.
*/
int netw_server_accept(Socket *serv, Socket *out, struct sockaddr_in *client);

#ifdef __cplusplus
}
#endif

#endif