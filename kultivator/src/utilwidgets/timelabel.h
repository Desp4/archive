#ifndef TIMELABEL_H
#define TIMELABEL_H

#include <QLabel>

// A Label class for dispaying time(not date)
class TimeLabel : public QLabel
{
public:
    TimeLabel(QWidget *parent = nullptr);

    qint64 getTime(){ return time; }

public slots:
    void setTime(qint64 seconds);

private:
    void printTime();

    qint64 time = -1;
};

#endif // TIMELABEL_H
