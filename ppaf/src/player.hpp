#ifndef AUDIO_PLAYER
#define AUDIO_PLAYER

#include <atomic>

#include "portaudio.h"

#include "buffer/audiobuffer.hpp"
#include "decstream.hpp"

constexpr AVSampleFormat PL_EXPORT fmtPAtoAV(PaSampleFormat fmt);

class PL_EXPORT AudioPlayer
{
public:
    /* rampus - volume ramp microseconds, buflen - buffer length in seconds, 
       undfw - seconds without sufficient data before the stream considers it an underflow and terminates.
       Default values are arbitrary.*/
    AudioPlayer(uint32_t rampus = 2000, double buflen = 1.0, double undfw = 1.0);
    virtual ~AudioPlayer();

    // Open UTF-8 filename, on Windows also supports UTF-16.
    virtual bool open(const FileName& path, bool autoplay = true);
    virtual void close();
    virtual void pause();
    virtual void play();

    virtual void setVolume(float vol);

    void seek(double timestamp);

protected:
    virtual int outputCallback(void** output, unsigned long frames,
        const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags flags);
    // Does nothing by default.
    virtual void finishedCallback();

    virtual void applyVolume(void** output, unsigned long frames);
    // Thread-safe write from a buffer
    void writeToStream(void** output, unsigned long frames);

    // All time values in seconds
    double duration = 0.0;
    double underflowTime = 0.0;
    std::atomic<double> currentTime = 0.0;
  
    AudioBuffer buffer;
    DecStream decoder;

    PaStreamParameters outStreamPar;
    PaStream* outStream = nullptr;

    float volume = 1.0f;
    float targetVolume = 1.0f;

private:
    static void paFinishedCallback(void* data);
    static int paCallback(
        const void* input,
        void* output,
        unsigned long frames,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags flags,
        void* data);
    

    // Only used in a destructor, so that PaTerminate() is not called.
    // If Pa_Init in a constructor fails, no functions mind it(PA code will fail though).
    bool good = false;
    bool haveFile = false;

    // For calculating time deltas
    double lastPaTime;

    // Volume delta per frame on volume ramp
    double rampDelta;

    // Time it takes to ramp volume from 0 to 1 in microseconds
    const uint32_t rampUS;
    // Buffer length expressed in terms of time(seconds)
    const double bufferDuration;
    const double underflowWait;
    // Locks writeToStream() and seek(), so that they don't collide while operating on a buffer
    std::mutex mutexCallback;
};
#endif
