#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QTreeView>

class PlaylistView : public QTreeView
{
    Q_OBJECT
public:
    PlaylistView(QWidget* parent = nullptr);

signals:
    void newFiles(const QMimeData* data);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

#endif
