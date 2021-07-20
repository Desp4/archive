#ifndef SLIDERSTYLE_H
#define SLIDERSTYLE_H

#include <QProxyStyle>

// used in SyncSlider : allows mouse clicks to move the slider
class SliderStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    int styleHint(QStyle::StyleHint hint, const QStyleOption *option = nullptr, const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const
    {
        if (hint == QStyle::SH_Slider_AbsoluteSetButtons)
            return Qt::LeftButton; // this specifies which buttons trigger slider pos change
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

#endif // SLIDERSTYLE_H
