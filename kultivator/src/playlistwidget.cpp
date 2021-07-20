#include "playlistwidget.h"
#include "ui_playlistwidget.h"

#include <QMimeData>
#include <QUrl>

#include <QDebug>
#include "tag/tag.h"

PlaylistWidget::PlaylistWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistWidget),
    library(this) // maybe not
{
    ui->setupUi(this);

    model = new PlaylistViewModel(&library,
                           {AudioData::Title,
                            AudioData::Artist,
                            AudioData::Album,
                            AudioData::Year},
                            this);

    ui->view_files->setModel(model);

    connect(&library, &AudioLib::inserted, this, &PlaylistWidget::insertInModel);

    connect(ui->view_files, &PlaylistView::newFiles, this, &PlaylistWidget::addFiles);
    connect(ui->view_files, &QTreeView::doubleClicked, this, &PlaylistWidget::selectFile);
    connect(ui->ledit_search, &QLineEdit::textChanged, this, &PlaylistWidget::searchFiles);
}

PlaylistWidget::~PlaylistWidget()
{
    //qDeleteAll(playlists);
    delete model;
    delete ui;
}

void PlaylistWidget::selectFile(const QModelIndex& index)
{
    QString path = ui->view_files->model()->data(index, CustomRole::GetFileRole).toString();
    emit fileSelected(path);
}

void PlaylistWidget::addFiles(const QMimeData* data)
{
    // NOTE: not ideal creating an array just to copy it.
    // When performing mime-checks that might be reasonable but without them nah
    QList<QUrl> urls = data->urls();
    std::vector<AudioFile> newFiles(urls.size());
    for (int i = 0; i < urls.size(); ++i)
        newFiles[i] = loadFile(urls[i].toLocalFile());

    library.insert(newFiles);
}

void PlaylistWidget::insertInModel(quint32 first, quint32 count)
{
    ui->view_files->model()->insertRows(first, count);
}

void PlaylistWidget::searchFiles(const QString& text)
{
    if (text == "")
    {
        model->removeSearchFilter();
        return;
    }
    // Tmp test for now
    std::vector<quint32> searchRes;

    for (int i = 0; i < library.size(); ++i)
    {
        auto it = library[i].artist.indexOf(text, 0, Qt::CaseInsensitive);

        if (it != -1)
            searchRes.push_back(i);
        else
        {
            it = library[i].title.indexOf(text, 0, Qt::CaseInsensitive);
            if (it != -1)
                searchRes.push_back(i);
            else
            {
                it = library[i].album.indexOf(text, 0, Qt::CaseInsensitive);
                if (it != -1)
                    searchRes.push_back(i);
            }
        }
    }
    model->setSearchFilter(searchRes);
}
