#ifndef STREAMDECMAN
#define STREAMDECMAN

#include <condition_variable>
#include <atomic>

#include "decoder.hpp"

class AudioBuffer;

class DecStream
{
public:
    DecStream(AudioBuffer* buffer = nullptr);
    ~DecStream();

    bool open(const FileName& file);
    // Stop the decoding thread, keep the file open
    void stop();
    // Close the file, stop the thread
    void close();
    // Start the decoding thread
    void run();

    void notify();
    void seek(double timestamp, bool flush = false);
    inline double duration();
    inline bool atEof();
    inline void setBuffer(AudioBuffer* buffer);
    inline void setFormat(const OutputFmt& format);

private:
    void decode();

    AudioDecoder decoder;
    AudioBuffer* buff;
    // holds AVERROR returned from libav
    int err = 0;
    bool haveFile = false;

    // Used to shut down the decoding thread
    std::atomic<bool> halt = false;
    // Used to skip expired samples after seek()
    std::atomic<bool> skip = false;

    std::thread decThread;

    std::mutex mutex;
    std::condition_variable conVar;
};


// Inlines
inline double DecStream::duration()
{
    return decoder.duration();
}

inline bool DecStream::atEof()
{
    return err == AVERROR_EOF;
}

inline void DecStream::setBuffer(AudioBuffer* buffer)
{
    buff = buffer;
}

inline void DecStream::setFormat(const OutputFmt& format)
{
    decoder.setOutputFmt(format);
}

#endif