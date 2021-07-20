#ifndef IO_CONTEXT
#define IO_CONTEXT

#include <cstdint>

#include "export.hpp"

#ifdef _WIN32
// Just include it whatever
#include <Windows.h>

typedef HANDLE FileHandle;

class PL_EXPORT FileName
{
public:
    FileName(const wchar_t* name);
    FileName(const char* name);
    ~FileName();

    operator const wchar_t* () const;

private:
    wchar_t* filename;
};

#else

#include <cstdio>

typedef FILE* FileHandle;
typedef const char* FileName;

#endif

struct AVIOContext;
struct AVFormatContext;

// Recommended buffer size for i/o contexts by ffmpeg docs
constexpr uint64_t BUFF_SIZE_DEF = 4096;

class PL_EXPORT IOContext {
public:
    IOContext(uint64_t buffersize = BUFF_SIZE_DEF);
    ~IOContext();

    // Return true on success
    bool initAVFmtCtx(const FileName& filename, AVFormatContext* fmtCtx);
    void closeIO();

protected:
    static int ioread(void* data, uint8_t* buf, int bufSize);
    static int64_t ioseek(void* data, int64_t pos, int whence);

private:
    uint8_t* buffer;
    uint64_t bufferSize;
    AVIOContext* ioCtx = nullptr;
    FileHandle file = nullptr;
};
#endif
