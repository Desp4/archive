#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <arpa/inet.h>
#endif

#include "netw.h"
#include "util.h"
#include "netdata.h"

#define CONNECTIONS_MAX 10
#define CLIENTS_MAX 20

typedef struct
{
	Socket socket;
	struct sockaddr_in addr;

	ClientData data;
} ClientCtx;

static struct
{
	Socket socket;

	uint8_t client_count;
	uint8_t id_glob;

	ClientCtx *clients;
	Thread *threads;
} g_chat_ctx;

int server_init(int clients_max)
{
	int ret = 0;
	Socket *socket = &g_chat_ctx.socket;

	if (ret = netw_server_create(socket, TCP))
		return ret;
	if (ret = netw_server_bind(socket, PORT))
		return ret;
	if (ret = netw_server_listen(socket, CONNECTIONS_MAX))
		return ret;

	int align = clients_max * sizeof(ClientCtx);
	g_chat_ctx.clients = malloc(align + clients_max * sizeof(Thread));
	g_chat_ctx.threads = (void *)g_chat_ctx.clients + align;

	g_chat_ctx.client_count = 0;
	g_chat_ctx.id_glob = 0;

	return ret;
}

TH_RET TH_CALL client_loop(void *data)
{
	ClientCtx *client = (ClientCtx *)data;
	char packet[256];
	ITEM_TYPE item_type = ITEM_CLIENTS;

	uint16_t size = sizeof(uint16_t) + sizeof(ITEM_TYPE);
	memcpy(packet + sizeof(uint16_t), &item_type, sizeof item_type);
	memcpy(packet + size, &g_chat_ctx.client_count, sizeof(uint8_t));
	size += sizeof(uint8_t);

	for (int i = 0; i < g_chat_ctx.client_count; ++i)
	{
		memcpy(packet + size, &g_chat_ctx.clients[i].data.id, sizeof(uint8_t));
		size += sizeof(uint8_t);
	}

	memcpy(packet, &size, sizeof size);
	send(client->socket, packet, size, 0);

	//recv(client->socket, packet, sizeof packet, 0);
}

int main()
{
	int ret = 0, len = IPV4_ADDRLEN;
	char ipbuf[IPV4_ADDRLEN];

	if (ret = netw_init())
	{
		log_err(ret);
		return 1;
	}
	
	if (ret = server_init(CLIENTS_MAX))
	{
		log_err(ret);
		// on fail need to do cleanup
		return 1;
	}

	// log server ip
	if (ret = netw_public_ip_v4(ipbuf, &len))
	{
		log_err(ret);
		// on fail need to do cleanup
		return 1;
	}
	printf("server created at %s:%i\n", ipbuf, PORT);

	// main accept loop
	while(1)
	{
		ClientCtx *client = &g_chat_ctx.clients[g_chat_ctx.client_count];
		ret = netw_server_accept(&g_chat_ctx.socket,
								 &client->socket,
								 &client->addr);
		if (ret)
		{
			log_err(ret);
			continue;
		}
		
		client->data.id = g_chat_ctx.id_glob++;

		if (ret = thread_spawn(client, client_loop, &g_chat_ctx.threads[g_chat_ctx.client_count]))
		{
			log_err(ret);
			continue;
		}

		++g_chat_ctx.client_count;
		// log connection
		inet_ntop(client->addr.sin_family,
				  &client->addr.sin_addr,
				  ipbuf,
				  sizeof ipbuf);
		printf("got connection from %s\n", ipbuf);
	}

	netw_terminate();
	return 0;
}