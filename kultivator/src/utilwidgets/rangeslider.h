#ifndef RANGESLIDER_H
#define RANGESLIDER_H

#include <QSlider>

// A Slider class with an internal value and range, mimicing float-like slider behavior
class RangeSlider : public QSlider
{
    Q_OBJECT
public:
    explicit RangeSlider(QWidget *parent = nullptr);

    // Set scrolling speed in local range units.
    void setScrollDelta(double delta);
    void setValueRange(double min, double max);
    // Reset current position to min, reset internal states. Doesn't emit a signal.
    void resetPos();

signals:
    void posChanged(double pos);

public slots:
    void setPos(double pos);

protected slots:
    void updatePos(int newValue);

protected:
    void wheelEvent(QWheelEvent *event) override;

    double minValue = 0;
    double maxValue = 1;
    double currentValue = minValue;
    double scrollDelta = 0;

    bool overridePos = true;
};

#endif // SYNCSLIDER_H
