#include "iocontext.hpp"

extern "C"
{
#include <libavformat/avformat.h>
}

#ifdef _WIN32

typedef DWORD FilePointer;

// FileName impl

FileName::FileName(const wchar_t* name)
{
    filename = new wchar_t[wcslen(name)];
    wcscpy(filename, name);
}

FileName::FileName(const char* name)
{
    int charSize = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
    filename = new wchar_t[charSize];
    MultiByteToWideChar(CP_UTF8, 0, name, -1, filename, charSize);
}

FileName::~FileName()
{
    delete[] filename;
}

FileName::operator const wchar_t* () const
{
    return filename;
}

// Win API

FileHandle openFile(const FileName& path)
{
    HANDLE ret = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    return ret != INVALID_HANDLE_VALUE ? ret : nullptr;
}

bool closeFile(FileHandle file)
{
    return !CloseHandle(file);
}

bool seekFile(FileHandle file, int64_t offset, int origin)
{
    // Origin macros match those of fseek
    return SetFilePointer(file, offset, NULL, origin) == INVALID_SET_FILE_POINTER;
}

FilePointer readFile(FileHandle file, void* buffer, FilePointer size)
{
    DWORD len;
    return ReadFile(file, buffer, size, &len, NULL) != 0 ? len : 0;
}

FilePointer filePos(FileHandle file)
{
    return SetFilePointer(file, 0, NULL, FILE_CURRENT);
}

#else

typedef uint64_t FilePointer;

FileHandle openFile(const FileName& path)
{
    return fopen(path, "rb");
}

bool closeFile(FileHandle file)
{
    return fclose(file);
}

bool seekFile(FileHandle file, int64_t offset, int origin)
{
    return fseek(file, offset, origin);
}

FilePointer readFile(FileHandle file, void* buffer, FilePointer size)
{
    return fread(buffer, 1, size, file);
}

FilePointer filePos(FileHandle file)
{
    return ftell(file);
}

#endif



IOContext::IOContext(uint64_t buffersize) :
    bufferSize(buffersize),
    buffer(static_cast<uint8_t*>(av_malloc(buffersize)))
{
}

IOContext::~IOContext()
{
    closeIO();
    if (buffer)
        av_freep(&buffer);
}

bool IOContext::initAVFmtCtx(const FileName& filename, AVFormatContext* fmtCtx)
{
    closeIO();
    file = openFile(filename);

    if (!file)
        return false;

    ioCtx = avio_alloc_context(
        buffer,         // Internal buffer
        bufferSize,     // Buffer size
        0,              // Write flag(1=true, 0=false) 
        this,           // User data, will be passed to the callback functions
        ioread,         // Read callback
        nullptr,        // No write callback
        ioseek);        // Seek callback


    fmtCtx->pb = ioCtx;
    fmtCtx->flags |= AVFMT_FLAG_CUSTOM_IO;

    // Read the file and let ffmpeg guess it.
    FilePointer len = readFile(file, buffer, bufferSize);
    if (!len)
        return false;

    // Seek to the beginning.
    seekFile(file, 0, SEEK_SET);

    // Set up a probe.
    AVProbeData probeData;
    probeData.buf = buffer;
    probeData.buf_size = len;
    probeData.filename = "";
    probeData.mime_type = NULL;

    fmtCtx->iformat = av_probe_input_format(&probeData, 1);

    return true;
}

void IOContext::closeIO()
{
    if (file)
    {
        closeFile(file);
        file = nullptr;
    }       
    if (ioCtx)
        avio_context_free(&ioCtx);
}

int IOContext::ioread(void* data, uint8_t* buf, int bufSize)
{
    IOContext* thisio = reinterpret_cast<IOContext*>(data);
    FilePointer len = readFile(thisio->file, buf, bufSize);
    if (!len)
        return AVERROR_EOF; // Probably reached EOF, let ffmpeg know.
    return len;
}

// Whence: SEEK_SET, SEEK_CUR, SEEK_END and AVSEEK_SIZE
int64_t IOContext::ioseek(void* data, int64_t pos, int whence)
{
    if (whence == AVSEEK_SIZE)
        return -1; // Don't support - return a negative.

    IOContext* thisio = reinterpret_cast<IOContext*>(data);
    if (seekFile(thisio->file, pos, whence))
        return -1;

    return filePos(thisio->file);
}
