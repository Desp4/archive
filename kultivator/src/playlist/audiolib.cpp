#include "audiolib.h"

quint32 AudioLib::size() const
{
    return files.size();
}

AudioFile& AudioLib::operator[](quint32 ind)
{
    return files[ind];
}

const AudioFile& AudioLib::operator[](quint32 ind) const
{
    return files[ind];
}

void AudioLib::insert(const std::vector<AudioFile>& in, quint32 pos)
{
    Q_UNUSED(pos);

    files.insert(files.end(),in.begin(), in.end());

    emit inserted(files.size() - in.size(), in.size());
}
