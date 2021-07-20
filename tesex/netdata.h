#ifndef KENT_PACKET_H
#define KENT_PACKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define PORT 4051
#define PACKET_SIZE 256

typedef uint8_t Byte;

typedef struct
{
	uint8_t id;
} ClientData;

// one byte is enough for now
#pragma pack(push, 1)
typedef enum
{
    ITEM_CLIENTS = 137
} ITEM_TYPE;
#pragma pack(pop)

/*
Pack client data.
If buf is not NULL, no bound checks are performed.

If buf is NULL return number of bytes to write, otherwise bytes written.
*/
uint16_t pack_clients(Byte *buf, ClientData *clients, uint8_t count);

#ifdef __cplusplus
}
#endif

#endif