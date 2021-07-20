#include <stdio.h>

#include "netw.h"
#include "util.h"
#include "netdata.h"

int main(int argc, char *argv[])
{
    char ipbuf[IPV4_ADDRLEN];
    char *ip_ptr;
    int ret;
    Socket sock;

    if (ret = netw_init())
	{
		log_err(ret);
		return 1;
	}

    if (argc < 2)
    {
        // should add a command and parse it later you know the drill
        fgets(ipbuf, sizeof ipbuf, stdin);
        ip_ptr = ipbuf;
    }
    else
    {
        ip_ptr = argv[1];
    }
    
    if (ret = netw_client_create(ip_ptr, PORT, TCP, &sock))
    {
        log_err(ret);
        // on fail need to do cleanup
		return 1;
    }

    char buf[256];
    int num = recv(sock, buf, sizeof buf, 0);
    uint8_t *iter = buf;
    uint16_t size = *((uint16_t *)iter);
    iter += sizeof size;
    ITEM_TYPE type = *((ITEM_TYPE *)iter);
    iter += sizeof type;
    uint8_t clients = *iter;
    iter += sizeof clients;

    printf("returned %i\n", num);
    printf("reported size: %i\n", size);
    printf("reported type: %i\n", (int)type);
    printf("reported clients: %i\n", (int)clients);

    for (int i = 0; i < clients; ++i)
    {
        uint8_t *id = iter;
        printf("client id %i: %i\n", i, (int)*id);
        iter += sizeof(uint8_t);
    }

    netw_socket_close(sock);
    return 0;
}