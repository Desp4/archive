#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMimeData>
#include <QUrl>

#include "playerwidget.h"
#include "tag/tag.h"

MainWindow::MainWindow(QWidget *parent) :
    CFramelessWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    player = ui->playerWidget;

    setTitleBar(player);
    QVector<QWidget*> whitelist = player->getWhiteList();
    for(QWidget* p : whitelist)
        addIgnoreWidget(p);

    connect(player, &PlayerWidget::closeClicked, this, &MainWindow::close); // crashed in win7
    connect(player, &PlayerWidget::minimizeClicked, this, &MainWindow::minimize);
    connect(player, &PlayerWidget::maximizeClicked, this, &MainWindow::maximize);

    connect(ui->tab_playlist, &PlaylistWidget::fileSelected, player, &PlayerWidget::openFile);

    /* https://stackoverflow.com/questions/38554640/add-border-under-column-headers-in-qtablewidget
    if(QSysInfo::windowsVersion()==QSysInfo::WV_WINDOWS10){
        setStyleSheet(
            "QHeaderView::section{"
                "border-top:0px solid #D8D8D8;"
                "border-left:0px solid #D8D8D8;"
                "border-right:1px solid #D8D8D8;"
                "border-bottom: 1px solid #D8D8D8;"
                "background-color:white;"
                "padding:4px;"
            "}"
            "QTableCornerButton::section{"
                "border-top:0px solid #D8D8D8;"
                "border-left:0px solid #D8D8D8;"
                "border-right:1px solid #D8D8D8;"
                "border-bottom: 1px solid #D8D8D8;"
                "background-color:white;"
            "}");}*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::maximize()
{
    setWindowState(isMaximized() ? windowState() & ~Qt::WindowMaximized :
                                   windowState() | Qt::WindowMaximized);
}

void MainWindow::minimize()
{
    setWindowState(windowState() | Qt::WindowMinimized);
}
