#ifndef PLAYLISTMASK_H
#define PLAYLISTMASK_H

#include <unordered_map>

#include "../tag/tag.h"

class AudioLib;

enum AudioData
{
    None,
    Title,
    Artist,
    Album,
    Year,
    Cover,
    Filename,
    Filepath
};

typedef std::vector<quint32> Mapping;
typedef std::unordered_map<AudioData, Mapping> Sorting;

#define PLAYLIST_NONE nullptr

// NOTE: renome unnecesaridsudgs pointers, do proper cleanup if not already

class PlaylistFilter
{
public:
    PlaylistFilter(AudioLib* lib);

    quint32 currentSize() const;

    void setPlaylistMask(const Mapping* playlistMask);
    void setSearchMask(const Mapping& searchMask);
    void setSorting(const AudioData tag, Qt::SortOrder order);

    void addSorting(const AudioData tag);
    void removeSorting(const AudioData tag);
    void updateSorting(quint32 row, quint32 count);

    void removeSearchMask();

    AudioFile& operator[](quint32 ind);
    const AudioFile& operator[](quint32 ind) const;

private:
    void sort(Mapping& dest, const Mapping* mask, const AudioData tag);
    void updateSort(Mapping& dest, const Mapping* mask, const AudioData tag, quint32 row, quint32 count);

    AudioLib* library;

    // playlist not owned
    const Mapping* playlist = PLAYLIST_NONE;
    Mapping search;

    Sorting sortsList;
    Sorting sortsSearch;

    struct
    {
        const Mapping* playlist;
        const Mapping* mask;
        Sorting* sort;
        AudioData tag;
        Qt::SortOrder order;
    } curr;
};

#endif
