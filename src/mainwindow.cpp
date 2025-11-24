#include "mainwindow.hpp"
// Qt
#include <QDirListing>
#include <QMediaMetaData>
#include <QScreen>
// TagLib
#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/flacfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/mp4coverart.h>
#include <taglib/mp4file.h>
#include <taglib/mp4item.h>
#include <taglib/mp4tag.h>
#include <taglib/mpegfile.h>

MainWindow::MainWindow()
	: m_centralWidget(new QWidget(this))
	, m_sideBarWidget(new QWidget())
	, m_playbackControlsWidget(new PlayerControls())
	, m_coverLabel(new QLabel())
	, m_middleTreeView(new QTreeView())
	, m_playerMainTreeView(new QTreeView())
	, m_sideBarLayout(new QVBoxLayout(m_sideBarWidget))
	, m_mainGridLayout(new QGridLayout(this))
	, m_audioPlayer(new QMediaPlayer(this))
	, m_audioOutput(new QAudioOutput(this)) {

		readWindowGeometrySettings();

		m_audioPlayer->setAudioOutput(m_audioOutput);

		m_playbackControlsWidget->setVolume(m_audioOutput->volume());
		m_playbackControlsWidget->setMuted(m_playbackControlsWidget->isMuted());

		connect(m_playbackControlsWidget, &PlayerControls::pause, m_audioPlayer, &QMediaPlayer::pause);
		connect(m_playbackControlsWidget, &PlayerControls::play, m_audioPlayer, &QMediaPlayer::play);

		connect(m_audioPlayer, &QMediaPlayer::durationChanged, m_playbackControlsWidget, &PlayerControls::setTrackDuration);
		connect(m_audioPlayer, &QMediaPlayer::positionChanged, m_playbackControlsWidget, &PlayerControls::setTrackProgress);
		connect(m_playbackControlsWidget, &PlayerControls::changeProgress, m_audioPlayer, &QMediaPlayer::setPosition);

		connect(m_playbackControlsWidget, &PlayerControls::changeVolume, m_audioOutput, &QAudioOutput::setVolume);
		connect(m_playbackControlsWidget, &PlayerControls::changeMuteState, m_audioOutput, &QAudioOutput::setMuted);

		connect(m_audioOutput, &QAudioOutput::volumeChanged, m_playbackControlsWidget, &PlayerControls::setVolume);
		connect(m_audioOutput, &QAudioOutput::mutedChanged, m_playbackControlsWidget, &PlayerControls::setMuted);

		scanLibrary();

		setupSideBar();
		setupPlayerModel();
		showLibrary();

		m_mainGridLayout->setSpacing(5);

		m_middleTreeView->header()->setDefaultAlignment(Qt::AlignCenter);
		m_playerMainTreeView->header()->setDefaultAlignment(Qt::AlignCenter);

		m_coverLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		m_coverLabel->setPixmap(m_coverImage);

		m_mainGridLayout->addWidget(m_playbackControlsWidget, 0, 0, 1, 33);
		m_mainGridLayout->addWidget(m_sideBarWidget, 1, 0, 13, 1);
		m_mainGridLayout->addWidget(m_middleTreeView, 1, 1, 9, 5);
		m_mainGridLayout->addWidget(m_coverLabel, 10, 1, 4, 5);
		m_mainGridLayout->addWidget(m_playerMainTreeView, 1, 6, 13, 27);

		connect(m_playerMainTreeView, &QTreeView::clicked, this, &MainWindow::rowClicked);

		m_centralWidget->setLayout(m_mainGridLayout);
		setCentralWidget(m_centralWidget);
}
void MainWindow::setupPlayerModel() {

		const QVariantList COLUMNS_NAME{"Number", "Title", "Album", "Author", "Duration", "Year", "Bitrate", "File Size", "Path"};

		m_playerModel = new TreeModel(m_tracks, COLUMNS_NAME);

		m_playerMainTreeView->setModel(m_playerModel);

		for (int32_t c = 0; c < m_playerModel->columnCount(QModelIndex()); ++c) {
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

void MainWindow::showAlbums() {
}

void MainWindow::showAuthors() {
}

void MainWindow::setupSideBar() {
		m_sideToolBar = addToolBar(tr("Side Tool Bar"));

		m_sideBarLayout->setSpacing(0);

		const QIcon LIBRARY_ICON = QIcon::fromTheme("focus-legacy-systray-symbolic");
		QAction *showLibraryAct = new QAction(LIBRARY_ICON, tr("&Library"), this);
		showLibraryAct->setStatusTip(tr("Library"));
		connect(showLibraryAct, &QAction::triggered, this, &MainWindow::showLibrary);
		m_sideToolBar->addAction(showLibraryAct);

		const QIcon PLAYLIST_ICON = QIcon::fromTheme("playlist-symbolic");
		QAction *showPlaylistsAct = new QAction(PLAYLIST_ICON, tr("&Playlists"), this);
		showPlaylistsAct->setStatusTip(tr("Playlists"));
		connect(showPlaylistsAct, &QAction::triggered, this, &MainWindow::showPlaylists);
		m_sideToolBar->addAction(showPlaylistsAct);

		const QIcon FAVORITE_ICON = QIcon::fromTheme("emblem-favorite-symbolic");
		QAction *showFavoriteAct = new QAction(FAVORITE_ICON, tr("&Favorite"), this);
		showFavoriteAct->setStatusTip(tr("Favorite"));
		connect(showFavoriteAct, &QAction::triggered, this, &MainWindow::showFavorite);
		m_sideToolBar->addAction(showFavoriteAct);

		const QIcon AUTHORS_ICON = QIcon::fromTheme("music-artist-symbolic");
		QAction *showAuthorsAct = new QAction(AUTHORS_ICON, tr("&Authors"), this);
		showAuthorsAct->setStatusTip(tr("Authors"));
		connect(showAuthorsAct, &QAction::triggered, this, &MainWindow::showAuthors);
		m_sideToolBar->addAction(showAuthorsAct);

		const QIcon FILES_ICON = QIcon::fromTheme("folder-symbolic");
		QAction *showFilesAct = new QAction(FILES_ICON, tr("&Files"), this);
		showFilesAct->setStatusTip(tr("Files"));
		connect(showFilesAct, &QAction::triggered, this, &MainWindow::showAlbums);
		m_sideToolBar->addAction(showFilesAct);

		m_sideToolBar->setOrientation(Qt::Vertical);
		m_sideBarLayout->addWidget(m_sideToolBar);
}

QPixmap MainWindow::getCoverArt(const QString &path, const QString &extension) {
		QPixmap img{};

		TagLib::FileName fileName{path.toUtf8().data()};

		if (extension == "mp3") {
				TagLib::MPEG::File file{fileName};
				TagLib::ID3v2::Tag *tag = file.ID3v2Tag(true);

				if (tag == nullptr) {
						return {":/Placeholders/Placeholder.svg"};
				}

				TagLib::ID3v2::FrameList frames = tag->frameList("APIC");

				if (frames.isEmpty()) {
						return {":/Placeholders/Placeholder.svg"};
				}

				auto *apic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front());

				QByteArray data(reinterpret_cast<const char *>(apic->picture().data()), apic->picture().size());
				img.loadFromData(reinterpret_cast<const uchar *>(data.constData()), data.size());

				return img;
		}
		if (extension == "mp4" || extension == "m4a") {
				TagLib::MP4::File file{fileName};
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

				QByteArray data(reinterpret_cast<const char *>(coverArt.data().data()), coverArt.data().size());

				img.loadFromData(reinterpret_cast<const uchar *>(data.constData()), data.size());

				return img;
		}
		if (extension == "flac") {
				TagLib::FLAC::File file{fileName};
				TagLib::ID3v2::Tag *tag = file.ID3v2Tag(true);

				if (tag == nullptr) {
						return {":/Placeholders/Placeholder.svg"};
				}

				TagLib::ID3v2::FrameList frames = tag->frameList("APIC");

				if (frames.isEmpty()) {
						return {":/Placeholders/Placeholder.svg"};
				}

				auto *apic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front());

				QByteArray data(reinterpret_cast<const char *>(apic->picture().data()), apic->picture().size());

				img.loadFromData(reinterpret_cast<const uchar *>(data.constData()), data.size());

				return img;
		}

		return {":/Placeholders/Placeholder.svg"};
}
void MainWindow::scanLibrary() {
		m_tracks.clear();

		QDir testLibrary{"../../../Muzyka"};

		Track track{};

		constexpr int32_t MIB = 1024 * 1024;
		constexpr auto FLAGS = QDirListing::IteratorFlag::Recursive | QDirListing::IteratorFlag::FilesOnly;

		for (const auto &file : QDirListing(testLibrary.path(), AUDIO_FILE_FILTER, FLAGS)) {
				TagLib::FileRef fileRef{file.filePath().toUtf8().data()};

				if (fileRef.tag() != nullptr) {
						const uint32_t NUMBER{fileRef.tag()->track()};
						TagLib::String title{fileRef.tag()->title().to8Bit(true)};
						TagLib::String album{fileRef.tag()->album().to8Bit(true)};
						TagLib::String artist{fileRef.tag()->artist().to8Bit(true)};
						const int32_t DURATION_IN_SECONDS{fileRef.audioProperties()->lengthInSeconds()};
						const uint32_t YEAR{fileRef.tag()->year()};
						const int32_t BITRATE{fileRef.audioProperties()->bitrate()};
						const QPixmap COVER_ART{getCoverArt(file.filePath(), file.suffix())};
						const QTime DURATION{0, DURATION_IN_SECONDS / 60, DURATION_IN_SECONDS % 60};
						m_coverImage = COVER_ART;

						track.number = NUMBER;
						track.title = title.toCString();
						track.album = album.toCString();
						track.artist = artist.toCString();
						track.duration = DURATION.toString("mm:ss");
						track.year = YEAR;
						track.bitrate = QString::number(BITRATE) + " kbps";
						track.fileSize = QString::number(file.size() / MIB) + " MiB";
						track.cover = COVER_ART;
						track.path = file.filePath();

						m_tracks.push_back(track);
				}
		}
}

void MainWindow::rowClicked(const QModelIndex &current) {
		QVariant data = TreeModel::dataAtColumn(current, Qt::DisplayRole, 9);

		m_coverImage = data.value<QPixmap>();
		m_coverLabel->setPixmap(m_coverImage);

		data = TreeModel::dataAtColumn(current, Qt::DisplayRole, 8);
		m_audioPlayer->setSource(QUrl::fromLocalFile(data.value<QString>()));
}