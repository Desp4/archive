#include "playlistfilter.h"

#include <functional>

#include "audiolib.h"

#define PAIR(tag, map) std::make_pair(tag, map)

constexpr AudioData TAG_DEFAULT = AudioData::Title;

std::function<bool(const AudioFile&, const AudioFile&)> tagToComparator(const AudioData type)
{
    // String comparison is not normalized
    switch (type)
    {
        case AudioData::Title    :
            return [](const AudioFile& val1, const AudioFile& val2)
            {
                return val1.title.localeAwareCompare(val2.title) < 0;
            };
        case AudioData::Artist   :
            return [](const AudioFile& val1, const AudioFile& val2)
            {
                return val1.artist.localeAwareCompare(val2.artist) < 0;
            };
        case AudioData::Album    :
            return [](const AudioFile& val1, const AudioFile& val2)
            {
                return val1.album.localeAwareCompare(val2.album) < 0;
            };
        case AudioData::Year     :
            return [](const AudioFile& val1, const AudioFile& val2)
            {
                return val1.year < val2.year;
            };

        default                  :
            return [](const AudioFile& val1, const AudioFile& val2)
            {
                Q_UNUSED(val1); Q_UNUSED(val2);
                return false;
            };
    }
}

PlaylistFilter::PlaylistFilter(AudioLib* lib) :
    library(lib)
{
    curr.tag = TAG_DEFAULT;
    curr.sort = &sortsList;
    curr.playlist = playlist;

    addSorting(curr.tag);
    curr.mask = curr.tag == AudioData::None ? curr.playlist : &(*curr.sort)[curr.tag];
}

quint32 PlaylistFilter::currentSize() const
{
    return curr.mask == PLAYLIST_NONE ? library->size() : curr.mask->size();
}

void PlaylistFilter::setPlaylistMask(const Mapping* playlistMask)
{
    playlist = playlistMask;

    for (auto p = sortsList.begin(); p != sortsList.end(); ++p)
        sort(p->second, playlist, p->first);

    // Force delete search, idealy shouldn't happen
    removeSearchMask();
}

void PlaylistFilter::setSearchMask(const Mapping& searchMask)
{
    search = searchMask;
    // Clear here in case I forget to guarantee that
    sortsSearch.clear();

    auto it = sortsSearch.insert(PAIR(curr.tag, Mapping())).first;
    // Technically can be deduced from current sorting without an actual sort
    // Not bothering with that
    sort(it->second, &search, it->first);

    curr.playlist = &search;
    curr.sort = &sortsSearch;
    curr.mask = curr.tag == AudioData::None ? curr.playlist : &(*curr.sort)[curr.tag];
}

void PlaylistFilter::setSorting(const AudioData tag, Qt::SortOrder order)
{
    addSorting(tag);
    curr.tag = tag;
    curr.order = order;
    curr.mask = curr.tag == AudioData::None ? curr.playlist : &(*curr.sort)[curr.tag];
}

void PlaylistFilter::addSorting(const AudioData tag)
{
    if (curr.sort->count(tag) != 0)
        return;

    auto it = curr.sort->insert(PAIR(tag, Mapping())).first;
    sort(it->second, curr.playlist, it->first);
}

void PlaylistFilter::removeSorting(const AudioData tag)
{
    // Not quite opposite to addSorting :
    // Removes sorting from both search and playlist
    if (sortsSearch.size() > 1 || sortsSearch.count(tag) != 0)
        sortsSearch.erase(sortsSearch.find(tag));

    if (sortsList.size() > 1 || sortsList.count(tag) != 0)
        sortsList.erase(sortsList.find(tag));

    // If removed current tag, switch to the first sorting
    if (curr.tag == tag)
    {
        curr.tag = curr.sort->begin()->first;
        curr.mask = curr.tag == AudioData::None ? curr.playlist : &(*curr.sort)[curr.tag];
    }
}

void PlaylistFilter::updateSorting(quint32 row, quint32 count)
{
    for (auto p = curr.sort->begin(); p != curr.sort->end(); ++p)
        updateSort(p->second, curr.playlist, p->first, row, count);
}

void PlaylistFilter::removeSearchMask()
{
    sortsSearch.clear();
    search.clear();

    if (sortsList.count(curr.tag) == 0)
    {
        auto it = sortsList.insert(PAIR(curr.tag, Mapping())).first;
        sort(it->second, playlist, it->first);
    }

    curr.playlist = playlist;
    curr.sort = &sortsList;
    curr.mask = curr.tag == AudioData::None ? curr.playlist : &(*curr.sort)[curr.tag];
}

AudioFile& PlaylistFilter::operator[](quint32 ind)
{
    quint32 realInd;
    if (curr.mask == PLAYLIST_NONE)
        realInd = curr.order == Qt::SortOrder::DescendingOrder ?
                    ind : library->size() - ind - 1;
    else
        realInd = curr.order == Qt::SortOrder::DescendingOrder ?
                    (*curr.mask)[ind] : (*curr.mask)[curr.mask->size() - ind - 1];

    return (*library)[realInd];
}

const AudioFile& PlaylistFilter::operator[](quint32 ind) const
{
    quint32 realInd;
    if (curr.mask == PLAYLIST_NONE)
        realInd = curr.order == Qt::SortOrder::DescendingOrder ?
                    ind : library->size() - ind - 1;
    else
        realInd = curr.order == Qt::SortOrder::DescendingOrder ?
                    (*curr.mask)[ind] : (*curr.mask)[curr.mask->size() - ind - 1];

    return (*library)[realInd];
}

void PlaylistFilter::sort(Mapping& dest, const Mapping* mask, const AudioData tag)
{
    if (tag == AudioData::None)
    {
        dest.clear();
        return;
    }

    if (mask == PLAYLIST_NONE)
    {
        dest.resize(library->size());
        dest.shrink_to_fit();
        std::iota(dest.begin(), dest.end(), 0);
    }
    else
        dest = *mask;

    auto compare = tagToComparator(tag);
    std::sort(dest.begin(), dest.end(),
              [&compare, this](quint32 i1, quint32 i2)
              {
                  return compare((*library)[i1], (*library)[i2]);
              });
}

void PlaylistFilter::updateSort(Mapping& dest, const Mapping* mask, const AudioData tag, quint32 row, quint32 count)
{
    if (tag == AudioData::None)
    {
        dest.clear();
        return;
    }

    auto compare = tagToComparator(tag);
    while (count > 0)
    {
        --count;
        const quint32 ind = mask == PLAYLIST_NONE ? row + count : (*mask)[row + count];
        auto it = std::lower_bound(dest.begin(), dest.end(), ind,
                                   [&compare, this](quint32 it, int val)
                                   {
                                       return compare((*library)[it], (*library)[val]);
                                   });
        dest.insert(it, ind);
    }
}
