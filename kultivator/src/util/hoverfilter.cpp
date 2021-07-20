#include "hoverfilter.h"

#include <QEvent>
#include <QWidget>
#include <QGraphicsOpacityEffect>

HoverFilter::HoverFilter(QObject* parent) :
    QObject(parent),
    activeTransp(1),
    inactiveTransp(0)
{
}

HoverFilter::HoverFilter(double active, double inactive, QObject* parent) :
    QObject(parent),
    activeTransp(active),
    inactiveTransp(inactive)
{
}


bool HoverFilter::eventFilter(QObject* object, QEvent* event)
{
    QWidget* widget = qobject_cast<QWidget*>(object);

    if (event->type() == QEvent::Enter)
    {
        // Set hover over.
       QGraphicsOpacityEffect* effect = dynamic_cast<QGraphicsOpacityEffect*>(widget->graphicsEffect());
       if (effect)
           effect->setOpacity(activeTransp);
       return true;
    }
    if (event->type() == QEvent::Leave)
    {
        // Return to normal.
        QGraphicsOpacityEffect* effect = dynamic_cast<QGraphicsOpacityEffect*>(widget->graphicsEffect());
        if (effect)
            effect->setOpacity(inactiveTransp);
        return true;
    }
    return false;
}
