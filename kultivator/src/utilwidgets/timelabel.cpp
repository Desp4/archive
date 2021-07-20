#include "timelabel.h"

TimeLabel::TimeLabel(QWidget *parent) :
    QLabel(parent)
{
}

void TimeLabel::setTime(qint64 seconds)
{
    if (time != seconds)
    {
        time = seconds;
        printTime();
    }
}

// Print time as M:SS > MM:SS > H:MM:SS > and further for hours field
void TimeLabel::printTime()
{
    int secs = time % 60;
    int mins = (time / 60) % 60;
    int hours = time / 3600;

    QString output("");
    if(time >= 3600)
    {
        output += QString::number(hours) + ":";
        if(mins < 10)
            output += "0";
    }
    output += QString::number(mins) + (secs < 10 ? ":0" : ":") + QString::number(secs);
    setText(output);
}
