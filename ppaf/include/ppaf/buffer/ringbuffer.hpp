#ifndef BUFFER_RING
#define BUFFER_RING

#include <cstdint>

#include "../export.hpp"

class PL_EXPORT RingBuffer
{
public:
    RingBuffer();
    RingBuffer(uint32_t charSize, uint64_t len);
    RingBuffer(const RingBuffer& buf);
    RingBuffer(RingBuffer&& buf);
    ~RingBuffer();

    // Returns bytes written
    uint32_t write(void* inData, uint64_t len);
    // Returns bytes extracted
    uint32_t extract(void* dest, uint64_t len);
    // Reset read and write pointers
    void flush();
    void resize(uint32_t charSize, uint64_t len);

    // Return number of elements in the buffer
    uint64_t readable();
    // Return number of elements that can be pushed into a buffer
    uint64_t writable();

    RingBuffer& operator=(const RingBuffer& buf);
    RingBuffer& operator=(RingBuffer&& buf);

private:
    uint64_t available();
    // Size of one element in bytes.
    uint32_t symbSize;
    // Array size in bytes
    uint64_t size;

    uint8_t* data = nullptr;
    uint8_t* end;

    // Pointer to next byte to write, end on init
    uint8_t* writePos;
    // Pointer to the next byte to read, data on init
    uint8_t* readPos;
};

#endif
