#include "playlistmodel.h"

#include "../tag/tag.h"
#include "../playlist/audiolib.h"

constexpr AudioData SORT_DEFAULT_TYPE = AudioData::Title;

// TAG LOOKUP FUNCTIONS

QVariant tagToData(const AudioFile& tag, AudioData type)
{
    switch (type)
    {
        case AudioData::Title    : return tag.title;
        case AudioData::Artist   : return tag.artist;
        case AudioData::Album    : return tag.album;
        case AudioData::Year     : return tag.year;
        case AudioData::Cover    : return tag.cover;
        case AudioData::Filename : return tag.file.fileName();
        case AudioData::Filepath : return tag.file.filePath();
        default                  : return "Nothing here!";
    }
}

QString tagToString(AudioData type)
{
    switch (type)
    {
        case AudioData::Title    : return "Title";
        case AudioData::Artist   : return "Artist";
        case AudioData::Album    : return "Album";
        case AudioData::Year     : return "Year";
        case AudioData::Cover    : return "Cover";
        case AudioData::Filename : return "Name";
        case AudioData::Filepath : return "Path";
        default                  : return "Nothing here!";
    }
}


// AUDIOMODEL IMPL

PlaylistViewModel::PlaylistViewModel(AudioLib* lib, QObject* parent) :
    PlaylistViewModel(lib, std::vector<AudioData>(1, SORT_DEFAULT_TYPE), parent)
{
}

PlaylistViewModel::PlaylistViewModel(AudioLib* lib, const std::vector<AudioData>& map, QObject* parent) :
    QAbstractItemModel(parent),
    filter(lib),
    tagMap(map)
{
}

PlaylistViewModel::~PlaylistViewModel()
{
}

void PlaylistViewModel::setTagMap(const std::vector<AudioData>& tags)
{
    // TODO: see what endInsert/remove do, maybe move the sorting before ending those
    int diff = tags.size() - tagMap.size();
    // Handle the view, add or remove columns in needed here
    if (diff > 0)
    {
        beginInsertColumns(QModelIndex(),
                           tagMap.size(),
                           tagMap.size() + diff - 1);
        tagMap = tags;
        endInsertColumns();
    }
    else if (diff < 0)
    {
        beginRemoveColumns(QModelIndex(),
                           tags.size(),
                           tags.size() - diff - 1);
        tagMap = tags;
        endRemoveColumns();
    }
    else
        tagMap = tags;

    // See if you need it on insertions and deletions
    emit headerDataChanged(Qt::Horizontal, 0, tags.size());
}

void PlaylistViewModel::setPlaylist(const Mapping* playlist)
{
    beginResetModel();

    filter.setPlaylistMask(playlist);

    endResetModel();
}

void PlaylistViewModel::setSearchFilter(const Mapping& search)
{
    beginResetModel();

    filter.setSearchMask(search);

    endResetModel();
}

void PlaylistViewModel::removeSearchFilter()
{
    beginResetModel();

    filter.removeSearchMask();

    endResetModel();
}

QVariant PlaylistViewModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // GetFileRole is requested by me, sending a full filepath
    if (role == CustomRole::GetFileRole)
        return tagToData(filter[index.row()], AudioData::Filepath);

    if (role != Qt::DisplayRole)
        return QVariant();

    return tagToData(filter[index.row()], tagMap[index.column()]);
}

Qt::ItemFlags PlaylistViewModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index);
}

QVariant PlaylistViewModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return tagToString(tagMap[section]);
    return QVariant();
}

QModelIndex PlaylistViewModel::index(int row, int column,
                        const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, nullptr);
}

QModelIndex PlaylistViewModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int PlaylistViewModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return filter.currentSize();
}

int PlaylistViewModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return tagMap.size();
}

bool PlaylistViewModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent);

    // In static order row doesn't really count
    beginInsertRows(QModelIndex(), row, row + count - 1);

    // update sorting in filter here
    filter.updateSorting(row, count);

    endInsertRows();

    // Don't need to emit dataChanged here, insertion refreshes everything
    return true;
}

void PlaylistViewModel::sort(int column, Qt::SortOrder order)
{
    if (column >= tagMap.size())
        return;

    filter.setSorting(tagMap[column], order);

    // emit a signal to update the data
    emit dataChanged(QModelIndex(), QModelIndex());
}

Qt::DropActions PlaylistViewModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
