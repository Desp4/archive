#include "playlistview.h"

#include <QDragEnterEvent>
#include <QMimeData>

PlaylistView::PlaylistView(QWidget* parent) :
    QTreeView(parent)
{
}

void PlaylistView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void PlaylistView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void PlaylistView::dropEvent(QDropEvent* event)
{
    emit newFiles(event->mimeData());
    event->acceptProposedAction();
}
