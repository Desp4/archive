#ifndef AUDIO_PLAYER_Q
#define AUDIO_PLAYER_Q

#include <QObject>
#include <QTimer>

#include "ppaf/player.hpp"

class AudioPlayerQ : public QObject, public AudioPlayer
{
    Q_OBJECT
public:
    AudioPlayerQ(QObject* parent = nullptr);

    void play() override;
    void pause() override;
    void setVolume(float vol) override;

    void queueSeek(double s);
    double currDuration() { return duration; }

signals:
    void atEof();
    void notify(double time);

    // Internal signals to get around threaded timer calls
    void stopTimer();
    void startTimer();

protected:
    int outputCallback(void** output, unsigned long frames,
                       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags flags) override;

private slots:
    void seekTimeout();
    void notifyTimeout();

private:
    QTimer notifyTimer;
    QTimer seekTimer;

    bool seekQueued = false;
};

#endif
