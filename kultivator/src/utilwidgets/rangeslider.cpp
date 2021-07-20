#include "rangeslider.h"

#include <QWheelEvent>

#include "../util/sliderstyle.h"
#include "../util/nmath.h"

constexpr int UNIT_SCROLL = 120;

RangeSlider::RangeSlider(QWidget *parent) :
    QSlider(parent)
{
    // Allow slider clicks.
    setStyle(new SliderStyle(style()));

    // Set range to max width only once and keep it this way.
    setRange(0, maximumWidth());
    connect(this, &QSlider::sliderPressed, this, [this]{ overridePos = false; }); // Block incoming pos changes.
    connect(this, &QSlider::sliderReleased, this, [this]{ overridePos = true; }); // Allow them.

    // Called internaly from clicks and setValue()
    connect(this, &QSlider::valueChanged, this, &RangeSlider::updatePos);
}

void RangeSlider::setScrollDelta(double delta)
{
     scrollDelta = (delta / UNIT_SCROLL * (maxValue - minValue)) * maximum();
}

void RangeSlider::setValueRange(double min, double max)
{
    if (min >= max)
        return;

    currentValue = nmath::lerp(min,
                               max,
                               nmath::invLerp(minValue, maxValue, currentValue));
    scrollDelta *= (maxValue - minValue) / (max - min);
    minValue = min;
    maxValue = max;
}

void RangeSlider::resetPos()
{
    const QSignalBlocker blocker(this);
    currentValue = minValue;
    setValue(0);
    overridePos = true;
}

void RangeSlider::setPos(double pos)
{
    if (!overridePos)
        return;

    currentValue = qBound(minValue, pos, maxValue);

    // Don't emit a signal.
    const QSignalBlocker blocker(this);
    setValue(maximum() * nmath::invLerp(minValue, maxValue, currentValue));
}

void RangeSlider::updatePos(int newValue)
{
    double t = static_cast<double>(newValue) / maximum();
    currentValue = nmath::lerp(minValue, maxValue, t);
    emit posChanged(currentValue);
}

void RangeSlider::wheelEvent(QWheelEvent *event)
{
    if (overridePos)
    {
        QPoint deltaDegrees = event->angleDelta();
        if (!deltaDegrees.isNull()) // pixel scroll not implemented(a MAC OS feature)
        {
            int yDelta = deltaDegrees.y() * (event->inverted() ? -1 : 1); // account for wheel inversion(apparently it exists)
            setValue(qBound(0, static_cast<int>(value() + yDelta * scrollDelta), maximum()));
        }
    }
    event->accept();
}
