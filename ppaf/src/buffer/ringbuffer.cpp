#include "ringbuffer.hpp"

#include <algorithm>

RingBuffer::RingBuffer()
{
}

RingBuffer::RingBuffer(uint32_t charSize, uint64_t len) :
    symbSize(charSize),
    size(charSize* len)
{
    data = new uint8_t[size];
    end = data + size;
    flush();
}

RingBuffer::RingBuffer(const RingBuffer& buf)
{
    *this = buf;
}

RingBuffer::RingBuffer(RingBuffer&& buf)
{
    *this = std::move(buf);
}

RingBuffer::~RingBuffer()
{
    if (data)
        delete[] data;
}

RingBuffer& RingBuffer::operator=(const RingBuffer& buf)
{
    if (this != &buf)
    {
        if (size != buf.size)
        {
            if (data)
                delete[] data;

            data = new uint8_t[buf.size];
            size = buf.size;
        }
        symbSize = buf.symbSize;
        end = data + size;
        writePos = data + (buf.writePos - buf.data);
        readPos = data + (buf.readPos - buf.data);
        std::copy(buf.data, buf.data + size, data);
    }
    return *this;
}

RingBuffer& RingBuffer::operator=(RingBuffer&& buf)
{
    if (this != &buf)
    {
        if (data)
            delete[] data;

        symbSize = buf.symbSize;
        size = buf.size;
        data = buf.data;
        end = buf.end;
        writePos = buf.writePos;
        readPos = buf.readPos;

        buf.data = nullptr;
    }
    return *this;
}

uint32_t RingBuffer::write(void* inData, uint64_t len)
{
    // How many bytes will it write, clamped at max capacity
    const uint64_t toWrite = std::min(len * symbSize, available());
    // Size to write before wrapping
    const uint64_t tmpSize = std::min(toWrite, static_cast<uint64_t>(end - writePos));

    const uint8_t* dataI = static_cast<const uint8_t*>(inData);
    // Source end of unwrapped copy
    const uint8_t* dataIWrapEnd = dataI + tmpSize;
    std::copy(dataI, dataIWrapEnd, writePos);

    // If wrapping around do again
    if (tmpSize != toWrite)
    {
        std::copy(dataIWrapEnd, dataI + toWrite, data);
        writePos = data + toWrite - tmpSize;
    }
    else
        writePos += toWrite;

    return toWrite;
}

uint32_t RingBuffer::extract(void* dest, uint64_t len)
{
    // Procedure same as in write()
    const uint64_t toRead = std::min(len * symbSize, size - available());
    const uint64_t tmpSize = std::min(toRead, static_cast<uint64_t>(end - readPos));

    uint8_t* destI = static_cast<uint8_t*>(dest);
    // On first iteration readPos points in out of index adress, nothing copied from there though
    std::copy(readPos, readPos + tmpSize, destI);

    if (tmpSize != toRead)
    {
        std::copy(data, data + toRead - tmpSize, destI + tmpSize);
        readPos = data + toRead - tmpSize;
    }
    else
        readPos += toRead;

    return toRead;
}

void RingBuffer::flush()
{
    writePos = data;
    readPos = end;
}

void RingBuffer::resize(uint32_t charSize, uint64_t len)
{
    if (data) 
        delete[] data;
    symbSize = charSize;
    size = charSize * len;
    data = new uint8_t[size];
    end = data + size;
    flush();

}

uint64_t RingBuffer::readable()
{
    return (size - available()) / symbSize;
}

uint64_t RingBuffer::writable()
{
    return available() / symbSize;
}

uint64_t RingBuffer::available()
{
    volatile uint8_t* vreadPos = readPos, * vwritePos = writePos;
    if (vwritePos <= vreadPos)
        return vreadPos - vwritePos;
    return (vreadPos - data) + (end - vwritePos);
}