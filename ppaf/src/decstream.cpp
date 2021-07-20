#include "decstream.hpp"

#include "buffer/audiobuffer.hpp"

DecStream::DecStream(AudioBuffer* buffer) :
    buff(buffer)
{
}

DecStream::~DecStream()
{
    close();
}

bool DecStream::open(const FileName& file)
{
    close();
    int ret = decoder.openFile(file);
    return haveFile = ret ? false : true;
}

void DecStream::stop()
{
    if (decThread.joinable())
    {
        halt = true;
        notify();
        decThread.join();
    }
    halt = false;
    err = 0;
}

void DecStream::close()
{
    stop();
    decoder.dropFile();
    haveFile = false;
}

void DecStream::run()
{
    if (!haveFile || !buff)
        return;

    stop();
    skip = false;
    decThread = std::thread(&DecStream::decode, this);
}

void DecStream::decode()
{
    std::unique_lock<std::mutex> lock(mutex);

    const DecBuffer& samples = decoder.buffer();
    while ((err = decoder.receiveSamples()) >= 0 && !halt)
    {
        conVar.wait(lock, [&samples, this] { return (samples.numSamples <= buff->writable()) || halt || skip; });

        if (!skip)
            buff->write(reinterpret_cast<void**>(samples.data), samples.numSamples);
        else
            skip = false;
    }
}

void DecStream::notify()
{
    conVar.notify_all();
}

void DecStream::seek(double timestamp, bool flush)
{
    if (!haveFile)
        return;

    std::unique_lock<std::mutex> lock(mutex);
    decoder.seek(timestamp);
    if (flush)
        buff->flush();

    // err negative only on errors and eof - meaning thread is done
    // then start it again after a seek
    if (err < 0)
        run();
    else
        skip = true;
   
    lock.unlock();

    notify();
}
