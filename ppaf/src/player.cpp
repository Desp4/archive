#include "player.hpp"

#include <algorithm>

#include "utils.hpp"

#undef min

constexpr PaSampleFormat SAMPLE_FMT = (paFloat32) | (paNonInterleaved);

constexpr AVSampleFormat fmtPAtoAV(PaSampleFormat fmt)
{
    AVSampleFormat ret = AV_SAMPLE_FMT_NONE;
    switch (fmt)
    {
    case paFloat32: ret = AV_SAMPLE_FMT_FLT; break;
    case paFloat32 | paNonInterleaved: ret = AV_SAMPLE_FMT_FLTP; break;

    case paInt32: ret = AV_SAMPLE_FMT_S32; break;
    case paInt32 | paNonInterleaved: ret = AV_SAMPLE_FMT_S32P; break;

    case paInt16: ret = AV_SAMPLE_FMT_S16; break;
    case paInt16 | paNonInterleaved: ret = AV_SAMPLE_FMT_S16P; break;

    case paUInt8: ret = AV_SAMPLE_FMT_U8; break;
    case paUInt8 | paNonInterleaved: ret = AV_SAMPLE_FMT_U8P; break;

    default: ret = AV_SAMPLE_FMT_NONE;
    }
    return ret;
}

AudioPlayer::AudioPlayer(uint32_t rampus, double buflen, double undfw) :
    decoder(&buffer),
    rampUS(rampus),
    bufferDuration(buflen),
    underflowWait(undfw)
{
    PaError ret = Pa_Initialize();
    if (ret != paNoError)
        fprintf(stderr, "Pa_Initialize failed with error code %d\n", ret);

    //Default PaParams init
    outStreamPar.device = Pa_GetDefaultOutputDevice();
    const PaDeviceInfo* devInfo = Pa_GetDeviceInfo(outStreamPar.device);
    outStreamPar.channelCount = devInfo->maxOutputChannels;
    outStreamPar.sampleFormat = SAMPLE_FMT;
    outStreamPar.suggestedLatency = devInfo->defaultHighOutputLatency;
    outStreamPar.hostApiSpecificStreamInfo = nullptr;

    buffer.resize(outStreamPar.channelCount, sizeof(float), devInfo->defaultSampleRate * bufferDuration);
    decoder.setFormat({ layoutFromChannels(outStreamPar.channelCount),
                        static_cast<int>(devInfo->defaultSampleRate),
                        fmtPAtoAV(outStreamPar.sampleFormat) });

    // Open stream
    rampDelta = 1000000 / (devInfo->defaultSampleRate * rampUS);

    ret = Pa_OpenStream(
        &outStream,                     // The stream
        nullptr,                        // Input params - none
        &outStreamPar,                  // Output params
        devInfo->defaultSampleRate,     // Sample rate
        paFramesPerBufferUnspecified,   // Amount of frames requested on each callback call
        paNoFlag,                       // No flags, clip out of range samples
        &AudioPlayer::paCallback,       // The callback
        this);                          // (void*)data in a callback
    Pa_SetStreamFinishedCallback(outStream, &AudioPlayer::paFinishedCallback);

    if (ret != paNoError)
        fprintf(stderr, "Pa_OpenStream failed with error code %d\n", ret);
    good = ret == paNoError ? true : false;
}

AudioPlayer::~AudioPlayer()
{
    if (good)
    {
        close();
        Pa_CloseStream(outStream);
        Pa_Terminate();
    }
}

void AudioPlayer::close()
{
    Pa_AbortStream(outStream);
    decoder.close();
    buffer.flush();

    currentTime = 0.0;
    underflowTime = 0.0;
    duration = 0.0;
}

bool AudioPlayer::open(const FileName& path, bool autoplay)
{
    close();
    haveFile = decoder.open(path);
    if (haveFile)
    {
        decoder.run();
        duration = decoder.duration();
        currentTime = 0.0;
        underflowTime = 0.0;
        if (autoplay)
            play();
    }

    return haveFile;
}

void AudioPlayer::pause()
{   
    if (Pa_IsStreamActive(outStream) == 1)
    {
        underflowTime = 0.0;
        Pa_StopStream(outStream);
    }
}

void AudioPlayer::play()
{
    if (haveFile && Pa_IsStreamActive(outStream) == 0)
    {
        volume = targetVolume;
        Pa_StopStream(outStream); // Need to call it because of the callback returns.
        lastPaTime = Pa_GetStreamTime(outStream);
        underflowTime = 0.0;
        Pa_StartStream(outStream);
    }    
}

void AudioPlayer::setVolume(float vol)
{
    if (vol >= 0.0f)
    {
        targetVolume = vol;
        // If stream is not active skip the ramping completely
        if (Pa_IsStreamActive(outStream) != 1)
            volume = targetVolume;
    }
}

void AudioPlayer::seek(double timestamp)
{
    if (!haveFile)
        return;
    
    std::unique_lock<std::mutex> lock(mutexCallback);

    decoder.seek(timestamp, true);
    currentTime = timestamp;
    underflowTime = 0.0;
}

int AudioPlayer::outputCallback(void** output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags flags)
{
    const uint64_t readable = buffer.readable();
    const double currPaTime = Pa_GetStreamTime(outStream);
    int ret = paContinue;

    if (readable >= frames)
    {
        underflowTime = 0.0;
        atomicFloatInc(currentTime, currPaTime - lastPaTime);

        writeToStream(output, frames);
        decoder.notify();
    }
    else if (decoder.atEof())
    {
        underflowTime = 0.0;
        atomicFloatInc(currentTime, currPaTime - lastPaTime);

        writeToStream(output, readable);
        decoder.notify();

        // Fill empty samples with silence
        float** channels = reinterpret_cast<float**>(output);
        const size_t len = (frames - readable) * sizeof(float);
        for (int c = 0; c < outStreamPar.channelCount; ++c)
            std::memset(channels[c] + readable, 0, len);

        ret = paComplete;
    }
    else if (underflowTime >= underflowWait)
    {
        underflowTime = 0.0;
        ret = paAbort;
    }
    else
    {
        // Increment underflow time
        underflowTime += currPaTime - lastPaTime;

        // Fill empty samples with silence
        float** channels = reinterpret_cast<float**>(output);
        const size_t len = frames * sizeof(float);
        for (int c = 0; c < outStreamPar.channelCount; ++c)
            std::memset(channels[c], 0, len);
    }

    lastPaTime = currPaTime;

    return ret;
}

void AudioPlayer::finishedCallback()
{
    return;
}

int AudioPlayer::paCallback(
    const void* input,
    void* output,
    unsigned long frames,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags flags,
    void* data)
{
    return reinterpret_cast<AudioPlayer*>(data)->outputCallback(reinterpret_cast<void**>(output), frames, timeInfo, flags);
}

void AudioPlayer::paFinishedCallback(void* data)
{
    reinterpret_cast<AudioPlayer*>(data)->finishedCallback();
}

void AudioPlayer::writeToStream(void** output, unsigned long frames)
{
    std::unique_lock<std::mutex> lock(mutexCallback);
    buffer.extract(output, frames);
    lock.unlock();

    applyVolume(output, frames);
}

void AudioPlayer::applyVolume(void** output, unsigned long frames)
{
    float** channels = reinterpret_cast<float**>(output);

    const int rampFrames = std::min((unsigned long)(std::abs(targetVolume - volume) / rampDelta + 1), frames);

    int fr = 0;
    // Not coherent array adressing is a shame
    for (; fr < rampFrames; ++fr, volume = moveto(volume, targetVolume, (float)rampDelta))
        for (int c = 0; c < outStreamPar.channelCount; ++c)
            channels[c][fr] *= volume;

    for (int c = 0; c < outStreamPar.channelCount; ++c)
        for (int f = fr; f < frames; ++f)
            channels[c][f] *= volume;
}
