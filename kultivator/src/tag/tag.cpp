#include "tag.h"

#include <QMimeDatabase>
#include <QUrl>

#include <taglib/tag.h>
// MPEG
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>
// FLAC
#include <taglib/flacfile.h>

using namespace TagLib;

static QMimeDatabase mimeDatabase;

void extractID3v2(ID3v2::Tag* tag, AudioFile* file)
{
    ID3v2::FrameList id3v2Frames = tag->frameListMap()["APIC"];
    if (!id3v2Frames.isEmpty())
    {
        // Getting only the first picture
        ID3v2::AttachedPictureFrame *picFrame =
                static_cast<ID3v2::AttachedPictureFrame*>(*id3v2Frames.begin());
        const QByteArray picData(picFrame->picture().data(), picFrame->picture().size());

        file->cover.loadFromData(picData);
    }
}

void extractID3v1(ID3v1::Tag* tag, AudioFile* file)
{

}

void extractAPE(APE::Tag* tag, AudioFile* file)
{

}

void extractXiph(Ogg::XiphComment* tag, AudioFile* file)
{

}

AudioFile loadFile(const QString& path)
{
    AudioFile ret;
    ret.file = QFile(path);

    File* file = nullptr;
    // WIN32 is a crybaby. This works, utf-8 not tested though
#ifdef WIN32
    std::wstring filepath = path.toStdWString();
#else
    std::string filepath = path.toStdString();
#endif

    QString mimeType = mimeDatabase.mimeTypeForFile(ret.file).name();
    if (mimeType == "audio/mpeg")
    {
        MPEG::File* mpegFile = new MPEG::File(filepath.c_str(), false);
        file = mpegFile;
        // Fallback: ID3v2 => ID3v1 => APE
        if (mpegFile->hasID3v2Tag())
            extractID3v2(mpegFile->ID3v2Tag(), &ret);
        else if (mpegFile->hasID3v1Tag())
            extractID3v1(mpegFile->ID3v1Tag(), &ret);
        else if (mpegFile->hasAPETag())
            extractAPE(mpegFile->APETag(), &ret);
    }
    if (mimeType == "audio/flac")
    {
        FLAC::File* flacFile = new FLAC::File(filepath.c_str(), false);
        file = flacFile;
        // Fallback: ID3v2 => ID3v1 => XiphComment
        if (flacFile->hasID3v2Tag())
            extractID3v2(flacFile->ID3v2Tag(), &ret);
        else if (flacFile->hasID3v1Tag())
            extractID3v1(flacFile->ID3v1Tag(), &ret);
        else if(flacFile->hasXiphComment())
            extractXiph(flacFile->xiphComment(), &ret);
    }

    if (file)
    {
        Tag* tag = file->tag();

        ret.album = tag->album().toCString(true);
        ret.title = tag->title().toCString(true);
        ret.artist = tag->artist().toCString(true);
        ret.year = tag->year();

        delete file;
    }

    return ret;
}
