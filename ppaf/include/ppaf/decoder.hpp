#ifndef AUDIO_DECODER
#define AUDIO_DECODER

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
}

#include "iocontext.hpp"

struct SwrContext;

// For convenience.
inline uint64_t PL_EXPORT layoutFromChannels(int channels)
{
    return av_get_default_channel_layout(channels);
}

struct PL_EXPORT OutputFmt
{
    uint64_t chLayout;
    int sampleRate;
    AVSampleFormat sampleFmt;
};

struct PL_EXPORT DecBuffer
{
    uint8_t** data = nullptr;
    int numSamples = 0;
    int numAlloc = 0;
};

class PL_EXPORT AudioDecoder
{
public:
    AudioDecoder();
    ~AudioDecoder();

    // Open UTF-8 or UTF-16 encoded path(latter for Windows only)
    int openFile(const FileName& path);
    // Close the file, clean up.
    void dropFile();
    // Seek to a timestamp seconds. Return a negative on error.
    int seek(double timestamp);
    /* Receive a new frame and get samples from it.
       Return a negative AVERROR on completion or error, > 0 on success.
       Decoded data can be accessed from buffer().*/
    int receiveSamples();

    // Return the duration in seconds, a negative on error.
    double duration();
    /* Return a const reference to a buffer with decoded samples.
       The reference doesn't change during lifetime(the fields do).*/
    inline const DecBuffer& buffer();

    int setOutputFmt(const OutputFmt& settings);
    inline OutputFmt getOutputFmt();

private:
    // Return samples converted or a negative error.
    int processFrame();

    // Allocated on a per file basis
    AVFormatContext* inFmtCtx = nullptr;
    AVCodecContext* inCodecCtx = nullptr;
    int inStreamIndex = -1;

    // Same instance across files
    SwrContext* swr;
    AVFrame* inFrame;
    AVPacket inPacket;
    IOContext ioCtx;
    OutputFmt swrOutput;

    DecBuffer sampleBuff;

    // Whether to start the receiveSamples 'routine' from the start or not
    bool newFrame = true;
};

// Inlines
inline const DecBuffer& AudioDecoder::buffer()
{
    return sampleBuff;
}

inline OutputFmt AudioDecoder::getOutputFmt()
{
    return swrOutput;
}

#endif
