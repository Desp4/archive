#include "playerq.h"

constexpr int NOTIFY_DEFAULT_TIME = 120;
constexpr int SEEK_DEFAULT_TIME = 240;

#define LOG100 4.60517018599

AudioPlayerQ::AudioPlayerQ(QObject* parent) :
    QObject(parent),
    AudioPlayer(),
    notifyTimer(this),
    seekTimer(this)
{
    notifyTimer.setInterval(NOTIFY_DEFAULT_TIME);
    seekTimer.setInterval(SEEK_DEFAULT_TIME);
    seekTimer.setSingleShot(true);

    connect(&notifyTimer, &QTimer::timeout, this, &AudioPlayerQ::notifyTimeout);
    connect(&seekTimer, &QTimer::timeout, this, &AudioPlayerQ::seekTimeout);

    connect(this, &AudioPlayerQ::stopTimer, this, [this]{ notifyTimer.stop(); }, Qt::QueuedConnection);
    connect(this, &AudioPlayerQ::startTimer, this, [this]{ notifyTimer.start(); }, Qt::QueuedConnection);
}

void AudioPlayerQ::play()
{
    // reset to the beginning at eof
    if (currentTime >= duration)
        seek(0.0);

    AudioPlayer::play();
    if (Pa_IsStreamActive(outStream) == 1)
        notifyTimer.start();
}

void AudioPlayerQ::pause()
{
    AudioPlayer::pause();
    notifyTimer.stop();
}

void AudioPlayerQ::setVolume(float vol)
{
    // 1 to 1 as in QAudio::ConvertVolume
    float logVol = vol > 0.99f ? 1 : -std::log(1 - vol) / LOG100;
    AudioPlayer::setVolume(logVol);
}

void AudioPlayerQ::queueSeek(double s)
{
    // Doesn't account for bad timestamp but other than that it's ok
    emit notify(s);

    if (seekTimer.isActive())
    {
        seekQueued = true;
        currentTime = s;
    }
    else
    {
        seek(s);
        seekTimer.start();
    }
}

int AudioPlayerQ::outputCallback(void** output, unsigned long frames,
                                 const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags flags)
{
    int ret = AudioPlayer::outputCallback(output, frames, timeInfo, flags);

    if (underflowTime > 0.0)
        emit stopTimer();
    else if (!notifyTimer.isActive())
        emit startTimer();

    if (ret == paComplete)
        emit atEof();
    return ret;
}

void AudioPlayerQ::notifyTimeout()
{
    // eh
    // Reasoning: 1. Perform a seek to timestamp t
    //              such that t - t0 < tt,
    //              t0 = total duration, tt = seek timer timeout interval
    //            2. Queue a next seek, before tt time passes, currTime is set
    //            3. Current pos is still at 1., currTime is eventually incremented past t0
    //            4. Out of bounds value transmitted to widgets or possible passed to seekTimeout
    emit notify(currentTime >= duration ? duration : currentTime);
}

void AudioPlayerQ::seekTimeout()
{
    if (seekQueued)
    {
        // ehhh
        seek(currentTime >= duration ? duration : currentTime);
        seekQueued = false;
        seekTimer.start();
    }
}
