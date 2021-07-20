#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include <QSortFilterProxyModel>

#include "itemmodel/playlistmodel.h"
#include "playlist/audiolib.h"

class AudioList;

namespace Ui {
class PlaylistWidget;
}

class PlaylistWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistWidget(QWidget *parent = nullptr);
    ~PlaylistWidget();

signals:
    void fileSelected(const QString& path);
    void filesAdded(const QMimeData* data);

private slots:
    void selectFile(const QModelIndex& index);
    void addFiles(const QMimeData* data);
    void insertInModel(quint32 first, quint32 count);
    void searchFiles(const QString& text);

private:
    Ui::PlaylistWidget *ui;

    AudioLib library;
    //std::vector<Playlist*> playlists;

    PlaylistViewModel* model;
};

#endif // PLAYLISTWIDGET_H
