#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "framelesswindow/framelesswindow.h"

class PlayerWidget;
struct AudioFile;
class QMimeData;


namespace Ui {
class MainWindow;
}

class MainWindow : public CFramelessWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void maximize();
    void minimize();

private:
    Ui::MainWindow *ui;
    PlayerWidget* player;
};

#endif // MAINWINDOW_H
