#include "netdata.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

uint16_t pack_clients(Byte *buf, ClientData *clients, uint8_t count)
{
    // on null return bytes to write
    if (buf == NULL)
    {
        return sizeof(uint16_t) + sizeof(ITEM_TYPE) + 
            sizeof(uint8_t) + count * sizeof(uint8_t);
    }

    uint16_t size = sizeof(uint16_t);
    ITEM_TYPE type = ITEM_CLIENTS;
    memcpy(buf + size, &type, sizeof type);
    size += sizeof type;
    memcpy(buf + size, &count, sizeof count);
    size += sizeof count;

    for (int i = 0; i < count; ++i)
    {
        memcpy(buf + size, &clients[i].id, sizeof(uint8_t));
        size += sizeof(uint8_t);
    }

    memcpy(buf, &size, sizeof size);
    return size;
}

#ifdef __cplusplus
}
#endif