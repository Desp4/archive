#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>

#include "playerq.h"

namespace Ui {
class PlayerWidget;
}

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = nullptr);
    ~PlayerWidget() override;

    QVector<QWidget*> getWhiteList();

signals:
    void closeClicked();
    void minimizeClicked();
    void maximizeClicked();

public slots:
    void openFile(const QString& path);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void playClicked(int state);

private:
    Ui::PlayerWidget *ui;

    AudioPlayerQ player;
};

#endif // PLAYERWIDGET_H
