#include "audiobuffer.hpp"

#include <limits>

void AudioBuffer::resize(uint32_t size, uint32_t charLen, uint64_t bufLen)
{
    // Does it deallocate memory? Does it shrink it to fit? Idk
    buffers.assign(size, { charLen, bufLen });
}

bool AudioBuffer::write(void** inData, uint64_t len)
{
    for (int c = 0; c < buffers.size(); ++c)
        buffers[c].write(inData[c], len);
    return len;
}

bool AudioBuffer::extract(void** dest, uint64_t len)
{
    for (int c = 0; c < buffers.size(); ++c)
        buffers[c].extract(dest[c], len);
    return len;
}

void AudioBuffer::flush()
{
    for (RingBuffer& buffer : buffers)
        buffer.flush();
}

uint64_t AudioBuffer::readable()
{
    uint64_t ret = std::numeric_limits<uint64_t>::max();
    for (RingBuffer& buffer : buffers)
    {
        uint64_t ri = buffer.readable();
        if (ri < ret)
            ret = ri;
    }
    return ret;
}

uint64_t AudioBuffer::writable()
{
    uint64_t ret = std::numeric_limits<uint64_t>::max();
    for (RingBuffer& buffer : buffers)
    {
        uint64_t wi = buffer.writable();
        if (wi < ret)
            ret = wi;
    }
    return ret;
}