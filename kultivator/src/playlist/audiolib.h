#ifndef AUDIOLIB_H
#define AUDIOLIB_H

#include <QObject>

#include "../tag/tag.h"

class AudioLib : public QObject
{
    Q_OBJECT

public:
    AudioLib(QObject* parent = nullptr) : QObject(parent) {}
    quint32 size() const;

    void insert(const std::vector<AudioFile>& in, quint32 pos = 0);

    AudioFile& operator[](quint32 ind);
    const AudioFile& operator[](quint32 ind) const;

signals:
    void inserted(quint32 first, quint32 count);
    //void removed(quint32 first, quint32 count);
    //void updated(???);

private:
    std::vector<AudioFile> files;
};

#endif
