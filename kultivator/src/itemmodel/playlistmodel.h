#ifndef AUDIOTREEMODEL_H
#define AUDIOTREEMODEL_H

#include <QAbstractItemModel>

#include "../playlist/playlistfilter.h"

struct AudioFile;

enum CustomRole
{
    GetFileRole = Qt::ItemDataRole::UserRole + 1
};

class PlaylistViewModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    PlaylistViewModel(AudioLib* lib, QObject* parent = nullptr);
    PlaylistViewModel(AudioLib* lib, const std::vector<AudioData>& map, QObject* parent = nullptr);
    ~PlaylistViewModel() override;

    void setPlaylist(const Mapping* playlist);
    void setSearchFilter(const Mapping& search);
    void removeSearchFilter();

    void setTagMap(const std::vector<AudioData>& tags);

    // Inherited members

    // Must-have const accessors
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;

    // Edits
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    // Sort
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    // Drops
    Qt::DropActions supportedDropActions() const override;

private:
    PlaylistFilter filter;

    std::vector<AudioData> tagMap;
};

#endif
