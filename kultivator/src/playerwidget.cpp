#include "playerwidget.h"
#include "ui_playerwidget.h"

#include <QGraphicsOpacityEffect>
#include <QDragEnterEvent>
#include <QMimeData>

#include "util/hoverfilter.h"

#include "tag/tag.h"

// placeholders for now
#define TIME_DELTA 2
#define VOL_DELTA .05
#define VOL_INIT .5

PlayerWidget::PlayerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerWidget)
{
    ui->setupUi(this);

    // Connect win tab buttons.
    connect(ui->btn_close, &QPushButton::clicked, this, [this]{ emit closeClicked(); });
    connect(ui->btn_max, &QPushButton::clicked, this, [this]{ emit maximizeClicked(); });
    connect(ui->btn_min, &QPushButton::clicked, this, [this]{ emit minimizeClicked(); });

    // Play button init.
    {
        MultistateButton* btnPlay = ui->btn_play;
        btnPlay->setStates(2, { ":/images/import/ibtn_play_c1.png",
                                ":/images/import/ibtn_pause_c1.png" });
        connect(btnPlay, &MultistateButton::stateChanged, this, &PlayerWidget::playClicked);

		// Install opacity filter
        QGraphicsOpacityEffect *fadeEffect = new QGraphicsOpacityEffect(btnPlay);
        fadeEffect->setOpacity(0);
        btnPlay->setGraphicsEffect(fadeEffect);
		// Install hover filter
        btnPlay->installEventFilter(new HoverFilter(btnPlay));
    }
    // Sliders init, delta should be a constant, volume should carry over on launches
    ui->sld_time->setScrollDelta(TIME_DELTA);
    ui->sld_volume->setScrollDelta(VOL_DELTA);
    ui->sld_volume->setPos(VOL_INIT);
	player.setVolume(VOL_INIT);

    // Connect player
    connect(ui->sld_time, &RangeSlider::posChanged, &player, &AudioPlayerQ::queueSeek);
    connect(ui->sld_volume, &RangeSlider::posChanged, &player, &AudioPlayerQ::setVolume);
    connect(&player, &AudioPlayerQ::notify, ui->sld_time, &RangeSlider::setPos, Qt::QueuedConnection); // Explicit Queued signal - pass to slider's thread
    connect(&player, &AudioPlayerQ::notify, ui->lbl_elapsed, &TimeLabel::setTime, Qt::QueuedConnection); // Same ^^

    connect(&player, &AudioPlayerQ::atEof, this, [this]() { ui->btn_play->setClickState(0); }, Qt::QueuedConnection);
}

PlayerWidget::~PlayerWidget()
{
    delete ui;
}

/* Tmp, called on EXTERNAL file passed, not queued files
   Should just delete this and pass an AudioFile here,
   in this case on-player drops should be handled elsewhere where the AudioFile is constructed and then passed to here.*/
void PlayerWidget::openFile(const QString& path)
{
    AudioFile tag = loadFile(path); // Loads a full res image, probably not a good idea for queued files

    ui->lbl_song->setText(tag.title);
    ui->lbl_artist->setText(tag.artist + " — " + tag.album);
    ui->lbl_cover->setPixmap(tag.cover.scaled(80, 80, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)); // Test on non 1:1 dimensions

    ui->lbl_elapsed->setTime(0);
    ui->sld_time->setPos(0);

    // State 0 - paused, state 1 - play
   player.open(path.toUtf8().data(), ui->btn_play->currState());

	// Maybe use taglib, also should have it if from queue(if ffmpeg is more reliable maybe do it anyway)
    double duration = player.currDuration();
    ui->lbl_total->setTime(duration);
    ui->sld_time->setValueRange(0, duration); // Floor the value, yes.
}

QVector<QWidget*> PlayerWidget::getWhiteList()
{
    return QVector<QWidget*>({ui->wid_low,
                             ui->wid_top,
                             ui->wid_win,
                             ui->wid_btns,
                             ui->wid_time,
                             ui->wid_ameta,
                             ui->lbl_volume,
                             ui->lbl_total,
                             ui->lbl_elapsed,
                             ui->lbl_song,
                             ui->lbl_artist});
}

void PlayerWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept any drop with an url, good keep it that way
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void PlayerWidget::dropEvent(QDropEvent *event)
{
    // And yes, no checks for now.
    // Only accepts the first file, for now separate from the playlist.
    openFile(event->mimeData()->urls().first().toLocalFile());
    event->acceptProposedAction();
}

void PlayerWidget::playClicked(int state)
{
    if (state == 0)
        player.play();
    else if (state == 1)
        player.pause();
}
