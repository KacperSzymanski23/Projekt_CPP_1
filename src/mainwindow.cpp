#include "mainwindow.hpp"
#include "icons.hpp"
// Qt
#include <QDirListing>
#include <QMediaMetaData>
#include <QScreen>
// TagLib
#include <taglib/fileref.h>

MainWindow::MainWindow()
	: m_centralWidget(new QWidget(this))
	, m_sideBarWidget(new SideBar(this))
//	, m_settingsButton(new QToolButton(this))
	, m_playbackControlsWidget(new PlayerControls(this))
//	, m_settingsDialog(new SettingsDialog(this))
	, m_settings(Settings("config.cfg"))
	, m_coverLabel(new QLabel(this))
	, m_middleTreeView(new QTreeView(this))
	, m_playerMainTreeView(new QTreeView(this))

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

		connect(m_audioPlayer, &QMediaPlayer::playbackStateChanged, m_playbackControlsWidget, [this](QMediaPlayer::PlaybackState arg) {
				m_playbackControlsWidget->setPlayerState(arg);
		});

		scanLibrary();

		setupPlayerModel();
		showLibrary();

		m_mainGridLayout->setSpacing(5);

		m_middleTreeView->header()->setDefaultAlignment(Qt::AlignCenter);
		m_playerMainTreeView->header()->setDefaultAlignment(Qt::AlignCenter);

		m_coverLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		m_coverLabel->setPixmap(m_coverImage);
		m_coverLabel->setScaledContents(true);

//		m_mainGridLayout->addWidget(m_settingsButton, 0, 0, 1, 1);
		m_mainGridLayout->addWidget(m_playbackControlsWidget, 0, 1, 1, 32);
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

		m_playerModel = new TreeModel(m_tracks, COLUMNS_NAME, this);

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
		if (!isMaximized()) {
				qsettings.setValue("pos", pos());
				qsettings.setValue("size", size());
		}

		qsettings.endGroup();

		QMainWindow::closeEvent(event);
}

void MainWindow::readWindowGeometrySettings() {
		QSettings qsettings{};

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


void MainWindow::scanLibrary() {
		m_tracks.clear();

		QDir library{QString::fromStdString(m_settings.getSettingsEntry("libraryDirectory"))};

		if (!library.exists()) {
				return;
		}

		constexpr float MIB = 1024.0F * 1024.0F;
		constexpr auto FLAGS = QDirListing::IteratorFlag::Recursive | QDirListing::IteratorFlag::FilesOnly;
		const QStringList AUDIO_FILE_FILTER = {"*.mp4", "*.mp3", "*.flac", "*.wav", "*.ogg", "*.opus", "*.m4a"}; // Wspierane typy plików

		for (const auto &file : QDirListing(library.path(), AUDIO_FILE_FILTER, FLAGS)) {
				TagLib::FileRef fileRef{file.filePath().toUtf8().data()};
				QFileInfo fileInfo(file.filePath());
				Track track{};

				extractMetadata(fileInfo.absoluteFilePath());
		}

}

void MainWindow::extractMetadata(const QString &filePath) {
	TagLib::FileRef fileRef{filePath.toUtf8().data()};
	QFileInfo fileInfo(filePath);

	if (fileRef.tag() != nullptr) {
		Track track{};
		constexpr float MIB = 1024.0F * 1024.0F;

		const uint32_t NUMBER{fileRef.tag()->track()};
		const TagLib::String TITLE{fileRef.tag()->title().to8Bit(true)};
		const TagLib::String ALBUM{fileRef.tag()->album().to8Bit(true)};
		const TagLib::String ARTIST{fileRef.tag()->artist().to8Bit(true)};
		const int32_t DURATION_IN_SECONDS{fileRef.audioProperties()->lengthInSeconds()};
		const uint32_t YEAR{fileRef.tag()->year()};
		const int32_t BITRATE{fileRef.audioProperties()->bitrate()};
		const QString COVER_ART_PATH{findCoverArt(fileInfo)};
		const QTime DURATION{0, DURATION_IN_SECONDS / 60, DURATION_IN_SECONDS % 60};


		const float FILE_SIZE = static_cast<float>(fileInfo.size()) / MIB;

		track.number = NUMBER;
		track.title = TITLE.toCString();
		track.album = ALBUM.toCString();
		track.artist = ARTIST.toCString();
		track.duration = DURATION.toString("mm:ss");
		track.year = YEAR;
		track.bitrate = QString::number(BITRATE) + " kbps";
		track.fileSize = QString::number(FILE_SIZE, 'f', 1) + " MiB";
		track.coverArtPath = COVER_ART_PATH;
		track.path = filePath;

		m_tracks.emplace_back(track);
	}

}
void MainWindow::rowClicked(const QModelIndex &current) {
		QVariant data = TreeModel::dataAtColumn(current, Qt::DisplayRole, 9);

		m_coverImage = data.value<QString>();
		m_coverLabel->setPixmap(m_coverImage);

		data = TreeModel::dataAtColumn(current, Qt::DisplayRole, 8);
		m_audioPlayer->setSource(QUrl::fromLocalFile(data.value<QString>()));
}

QString MainWindow::findCoverArt(const QFileInfo &fileInfo) {
		constexpr auto FLAGS = QDirListing::IteratorFlag::Default;

		const QStringList IMAGE_FILE_FILTER = {"*.jpg", "*.jpeg", "*.png", "*.webp"}; // Wspierane typy plików dla grafiki okładki

		for (const auto &file : QDirListing(fileInfo.absolutePath(), IMAGE_FILE_FILTER, FLAGS)) {
				if (file.baseName() == "cover") {
						return file.absoluteFilePath();
				}
		}

		return nullptr;
}