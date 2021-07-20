#ifndef CHANNEL_ARR
#define CHANNEL_ARR

#include <vector>

#include "ringbuffer.hpp"

class PL_EXPORT AudioBuffer
{
public:
    void resize(uint32_t size, uint32_t charLen, uint64_t bufLen);
    bool write(void** inData, uint64_t len);
    bool extract(void** dest, uint64_t len);
    void flush();

    // Smallest maximum readable elements across all buffers
    uint64_t readable();
    // Smallest maximum writable elements across all buffers
    uint64_t writable();

private:
    std::vector<RingBuffer> buffers;
};

#endif