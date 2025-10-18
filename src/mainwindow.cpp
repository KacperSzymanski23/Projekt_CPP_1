#include "mainwindow.hpp"
#include "treemodel.hpp"
// Qt
#include <QDirListing>
#include <QScreen>
// TagLib
#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/flacfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/mp4file.h>
#include <taglib/mpegfile.h>

MainWindow::MainWindow()
	: m_coverLabel(new QLabel())
	, m_centralWidget(new QWidget(this))
	, m_sideBarWidget(new QWidget())
	, m_playbackControllWidget(new QWidget())
	, m_middleTreeView(new QTreeView())
	, m_playerMainTreeView(new QTreeView())
	, m_sideBarLayout(new QVBoxLayout(m_sideBarWidget))
	, m_mainGridLayout(new QGridLayout(this))
	, m_playbackControllLayout(new QGridLayout(m_playbackControllWidget)) {

		readWindowGeometrySettings();

		scanLibrarty();

		setupSideBar();
		setupPlaybackControll();
		setupPlayerModel();
		showLibrary();

		m_mainGridLayout->setSpacing(0);

		m_middleTreeView->header()->setDefaultAlignment(Qt::AlignCenter);
		m_playerMainTreeView->header()->setDefaultAlignment(Qt::AlignCenter);

		m_coverLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		m_coverLabel->setPixmap(m_coverImage);

		m_mainGridLayout->addWidget(m_sideBarWidget, 0, 0, 13, 1);
		m_mainGridLayout->addWidget(m_middleTreeView, 0, 1, 9, 5);
		m_mainGridLayout->addWidget(m_coverLabel, 9, 1, 4, 5);
		m_mainGridLayout->addWidget(m_playerMainTreeView, 0, 6, 13, 27);

		m_centralWidget->setLayout(m_mainGridLayout);
		setCentralWidget(m_centralWidget);
}

void MainWindow::setupPlayerModel() {

		const QVariantList COLUMNS_NAME{"Number", "Title", "Album", "Author", "Duration", "Year", "Bitrate", "File Size", "Path"};

		TreeModel *playerModel = new TreeModel(m_tracks, COLUMNS_NAME);

		m_playerMainTreeView->setModel(playerModel);

		for (int32_t c = 0; c < playerModel->columnCount(); ++c) {
				m_playerMainTreeView->resizeColumnToContents(c);
		}

		m_playerMainTreeView->show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
		QSettings qsettings{};

		qsettings.beginGroup("Geometry");

		qsettings.setValue("geometry", saveGeometry());
		qsettings.setValue("savestate", saveState());
		qsettings.setValue("maximized", isMaximized());
		if (!isMaximized()) {
				qsettings.setValue("pos", pos());
				qsettings.setValue("size", size());
		}

		qsettings.endGroup();

		QMainWindow::closeEvent(event);
}

void MainWindow::readWindowGeometrySettings() {
		QSettings qsettings;

		qsettings.beginGroup("Geometry");

		restoreGeometry(qsettings.value("geometry", saveGeometry()).toByteArray());
		restoreState(qsettings.value("savestate", saveState()).toByteArray());
		move(qsettings.value("pos", pos()).toPoint());
		resize(qsettings.value("size", size()).toSize());

		if (qsettings.value("maximized", isMaximized()).toBool()) {
				showMaximized();
		}

		qsettings.endGroup();
}

void MainWindow::showLibrary() {
}

void MainWindow::showPlaylists() {
}

void MainWindow::showFavorite() {
}

void MainWindow::showFiles() {
}

void MainWindow::showAuthors() {
}

void MainWindow::setupPlaybackControll() {
		QToolBar *playbackControllToolBar = addToolBar(tr("Playback Controll"));

		m_playbackControllLayout->setSpacing(0);

		const QIcon LOOP_ICON = QIcon::fromTheme("media-playlist-repeat-rtl-symbolic");
		QAction *loopPlaybackAct = new QAction(LOOP_ICON, tr("&Loop"), this);
		loopPlaybackAct->setStatusTip(tr("Loop Playback"));
		loopPlaybackAct->setCheckable(true);
		playbackControllToolBar->addAction(loopPlaybackAct);

		const QIcon SHUFFLE_ICON = QIcon::fromTheme("media-playlist-shuffle-symbolic");
		QAction *shuffleAct = new QAction(SHUFFLE_ICON, tr("&Shuffle"), this);
		shuffleAct->setStatusTip(tr("Shuffle"));
		shuffleAct->setCheckable(true);
		playbackControllToolBar->addAction(shuffleAct);

		const QIcon PREVIOUS_SONG_ICON = QIcon::fromTheme("media-skip-backward-symbolic");
		QAction *previousSongAct = new QAction(PREVIOUS_SONG_ICON, tr("&Previous"), this);
		previousSongAct->setStatusTip(tr("Previous Song"));
		playbackControllToolBar->addAction(previousSongAct);

		const QIcon START_ICON = QIcon::fromTheme("media-playback-start-symbolic");
		const QIcon PAUSE_ICON = QIcon::fromTheme("media-playback-pause-symbolic");
		QAction *startpauseAct = new QAction(START_ICON, tr("&Start"), this);
		startpauseAct->setStatusTip(tr("Start"));
		playbackControllToolBar->addAction(startpauseAct);

		const QIcon NEXT_SONG_ICON = QIcon::fromTheme("media-skip-forward-symbolic");
		QAction *nextSongAct = new QAction(NEXT_SONG_ICON, tr("&Next"), this);
		nextSongAct->setStatusTip(tr("Next Song"));
		playbackControllToolBar->addAction(nextSongAct);

		m_playbackSlider = new QSlider();
		m_playbackSlider->setOrientation(Qt::Horizontal);
		playbackControllToolBar->addWidget(m_playbackSlider);

		m_timeLabel = new QLabel(tr(" 00:00/00:00 "));
		playbackControllToolBar->addWidget(m_timeLabel);

		const QIcon MUTED_ICON = QIcon::fromTheme("audio-volume-muted-symbolic");
		const QIcon HIGH_VOLUME_ICON = QIcon::fromTheme("audio-volume-high-symbolic");
		const QIcon MED_VOLUME_ICON = QIcon::fromTheme("audio-volume-medium-symbolic");
		const QIcon LOW_VOLUME_ICON = QIcon::fromTheme("audio-volume-low-symbolic");
		QAction *audioControllAct = new QAction(HIGH_VOLUME_ICON, tr("&Volume"), this);
		nextSongAct->setStatusTip(tr("Volume"));
		playbackControllToolBar->addAction(audioControllAct);

		m_volumeSlider = new QSlider();
		m_volumeSlider->setOrientation(Qt::Horizontal);
		m_volumeSlider->setMaximumWidth(175);
		playbackControllToolBar->addWidget(m_volumeSlider);

		m_volumeLabel = new QLabel(tr(" 100 "));
		playbackControllToolBar->addWidget(m_volumeLabel);

		const QIcon FAVORITE_ICON = QIcon::fromTheme("emblem-favorite-symbolic");
		QAction *addFavoriteAct = new QAction(FAVORITE_ICON, tr("&Favorite"), this);
		addFavoriteAct->setStatusTip(tr("Add Favorite"));
		addFavoriteAct->setCheckable(true);
		playbackControllToolBar->addAction(addFavoriteAct);

		playbackControllToolBar->setOrientation(Qt::Horizontal);
}

void MainWindow::setupSideBar() {
		QToolBar *sideToolBar = addToolBar(tr("Side Tool Bar"));

		m_sideBarLayout->setSpacing(0);

		const QIcon LIBRARY_ICON = QIcon::fromTheme("focus-legacy-systray-symbolic");
		QAction *showLibratyAct = new QAction(LIBRARY_ICON, tr("&Library"), this);
		showLibratyAct->setStatusTip(tr("Library"));
		connect(showLibratyAct, &QAction::triggered, this, &MainWindow::showLibrary);
		sideToolBar->addAction(showLibratyAct);

		const QIcon PLAYLIST_ICON = QIcon::fromTheme("playlist-symbolic");
		QAction *showPlaylistsAct = new QAction(PLAYLIST_ICON, tr("&Playlists"), this);
		showPlaylistsAct->setStatusTip(tr("Playlists"));
		connect(showPlaylistsAct, &QAction::triggered, this, &MainWindow::showPlaylists);
		sideToolBar->addAction(showPlaylistsAct);

		const QIcon FAVORITE_ICON = QIcon::fromTheme("emblem-favorite-symbolic");
		QAction *showFavoriteAct = new QAction(FAVORITE_ICON, tr("&Favorite"), this);
		showFavoriteAct->setStatusTip(tr("Favorite"));
		connect(showFavoriteAct, &QAction::triggered, this, &MainWindow::showFavorite);
		sideToolBar->addAction(showFavoriteAct);

		const QIcon AUTHORS_ICON = QIcon::fromTheme("music-artist-symbolic");
		QAction *showAuthorsAct = new QAction(AUTHORS_ICON, tr("&Authors"), this);
		showAuthorsAct->setStatusTip(tr("Authors"));
		connect(showAuthorsAct, &QAction::triggered, this, &MainWindow::showAuthors);
		sideToolBar->addAction(showAuthorsAct);

		const QIcon FILES_ICON = QIcon::fromTheme("folder-symbolic");
		QAction *showFilesAct = new QAction(FILES_ICON, tr("&Files"), this);
		showFilesAct->setStatusTip(tr("Files"));
		connect(showFilesAct, &QAction::triggered, this, &MainWindow::showFiles);
		sideToolBar->addAction(showFilesAct);

		sideToolBar->setOrientation(Qt::Vertical);
		m_sideBarLayout->addWidget(sideToolBar);
}

void MainWindow::defaultAction() {
		qDebug() << "Click!";
}

void MainWindow::scanLibrarty() {
		m_tracks.clear();

		QDir testLibrary{""};

		Track track{};

		const int32_t MIB = 1024 * 1024;
		const auto FLAGS = QDirListing::IteratorFlag::Recursive | QDirListing::IteratorFlag::FilesOnly;

		for (const auto &file : QDirListing(testLibrary.path(), AUDIO_FILE_FILTER, FLAGS)) {
				TagLib::FileRef fileRef{file.filePath().toUtf8().data()};

				uint32_t number{fileRef.tag()->track()};
				TagLib::String title{fileRef.tag()->title().to8Bit(true)};
				TagLib::String album{fileRef.tag()->album().to8Bit(true)};
				TagLib::String artist{fileRef.tag()->artist().to8Bit(true)};
				int32_t durationInSeconds{fileRef.audioProperties()->lengthInSeconds()};
				uint32_t year{fileRef.tag()->year()};
				int32_t bitrate{fileRef.audioProperties()->bitrate()};
				QPixmap coverArt{getCoverArt(file.filePath(), file.suffix())};
				QTime duration{0, durationInSeconds / 60, durationInSeconds % 60};
				m_coverImage = coverArt;

				track.number = number;
				track.title = title.toCString();
				track.album = album.toCString();
				track.artist = artist.toCString();
				track.duration = duration.toString("mm:ss");
				track.year = year;
				track.bitrate = QString::number(bitrate) + " kbps";
				track.fileSize = QString::number(file.size() / MIB) + " MiB";
				track.cover = coverArt;
				track.path = file.filePath();

				m_tracks.push_back(track);
		}
}

QPixmap MainWindow::getCoverArt(const QString &path, const QString &extension) {
		QPixmap img{};

		if (extension == "mp3") {
				TagLib::MPEG::File file(path.toStdString().c_str());
				TagLib::ID3v2::Tag *tag = file.ID3v2Tag(true);

				if (tag == nullptr) {
						return {":/Placeholders/Placeholder.svg"};
				}

				TagLib::ID3v2::FrameList frames = tag->frameList("APIC");

				if (frames.isEmpty()) {
						return {":/Placeholders/Placeholder.svg"};
				}

				auto *apic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front());

				QByteArray data((const char *)apic->picture().data(), apic->picture().size());
				img.loadFromData(reinterpret_cast<const uchar *>(data.constData()), data.size());

				return img;
		}
		if (extension == "mp4" || extension == "m4a") {
				TagLib::MP4::File file(path.toStdString().c_str());
				TagLib::MP4::Tag *tag = file.tag();

				if (tag == nullptr) {
						return {":/Placeholders/Placeholder.svg"};
				}

				TagLib::MP4::ItemMap itemsListMap = tag->itemMap();
				TagLib::MP4::Item coverItem = itemsListMap["covr"];

				TagLib::MP4::CoverArtList coverArtList = coverItem.toCoverArtList();
				if (coverArtList.isEmpty()) {
						return {":/Placeholders/Placeholder.svg"};
				}
				const TagLib::MP4::CoverArt &coverArt = coverArtList.front();

				QByteArray data((const char *)coverArt.data().data(), coverArt.data().size());

				img.loadFromData(reinterpret_cast<const uchar *>(data.constData()), data.size());

				return img;
		}
		if (extension == "flac") {
				TagLib::FLAC::File file(path.toStdString().c_str());
				TagLib::ID3v2::Tag *tag = file.ID3v2Tag(true);

				if (tag == nullptr) {
						return {":/Placeholders/Placeholder.svg"};
				}

				TagLib::ID3v2::FrameList frames = tag->frameList("APIC");

				if (frames.isEmpty()) {
						return {":/Placeholders/Placeholder.svg"};
				}

				auto *apic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front());

				QByteArray data((const char *)apic->picture().data(), apic->picture().size());

				img.loadFromData(reinterpret_cast<const uchar *>(data.constData()), data.size());

				return img;
		}

		return {":/Placeholders/Placeholder.svg"};
}