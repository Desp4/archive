#ifndef HOVERFILTER_H
#define HOVERFILTER_H

#include <QObject>

// A hover filter specifically for a play button - changing it's transparency on hover over
class HoverFilter : public QObject
{
    Q_OBJECT
public:
    explicit HoverFilter(QObject* parent = nullptr);
    HoverFilter(double active, double inactive, QObject *parent = nullptr);
    void setTransparency(double active, double inactive);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    double activeTransp;
    double inactiveTransp;
};

#endif // HOVERFILTER_H
