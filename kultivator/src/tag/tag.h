#ifndef KTAG_H
#define KTAG_H

#include <QFileInfo>
#include <QPixmap>

#define TAGLIB_STATIC

struct AudioFile
{
    QFileInfo file;

    // Text fields are UTF-8 encoded byte arrays
    QString title;
    QString artist;
    QString album;
    int year;

    QPixmap cover;
};

AudioFile loadFile(const QString& path);
#endif
