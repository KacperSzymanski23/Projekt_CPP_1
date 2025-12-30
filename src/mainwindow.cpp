#include "mainwindow.hpp"
#include "icons.hpp"
// Qt
#include <QAction>
#include <QDirListing>
#include <QFuture>
#include <QInputDialog>
#include <QMediaMetaData>
#include <QMenu>
#include <QScreen>
#include <QtConcurrent>
// TagLib
#include <taglib/fileref.h>
// Tracy
#include <tracy/Tracy.hpp>

MainWindow::MainWindow()
	: m_playbackQueue(new PlaybackQueue(this))
	, m_centralWidget(new QWidget(this))
	, m_sideBarWidget(new SideBar(this))
	, m_playbackControlsWidget(new PlayerControls(this))
	, m_settings(Settings("config.cfg"))
	, m_coverLabel(new QLabel(this))
	, m_middleTreeView(new QTreeView(this))
	, m_playerMainTreeView(new QTreeView(this))
	, m_mainGridLayout(new QGridLayout(this))
	, m_audioPlayer(new QMediaPlayer(this))
	, m_audioOutput(new QAudioOutput(this))
	, m_middleModel(new QStandardItemModel(this)) {
		ZoneScoped;

		readWindowGeometrySettings();

		m_audioPlayer->setAudioOutput(m_audioOutput);

		m_playbackControlsWidget->setVolume(m_audioOutput->volume());
		m_playbackControlsWidget->setMuted(m_playbackControlsWidget->isMuted());

		connect(m_sideBarWidget, &SideBar::showLibraryClicked, this, &MainWindow::showLibrary);
		connect(m_sideBarWidget, &SideBar::showPlaylistsClicked, this, &MainWindow::showPlaylists);

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

		connect(m_sideBarWidget, &SideBar::settingsChanged, this, &MainWindow::showLibrary);
		connect(m_sideBarWidget, &SideBar::settingsChanged, this, [this]() { m_settings.loadSettings(); });

		showLibrary();

		connect(m_playbackControlsWidget, &PlayerControls::next, m_playbackQueue, [this]() {
				m_playbackQueue->next();
				m_audioPlayer->play();
		});
		connect(m_playbackControlsWidget, &PlayerControls::previous, m_playbackQueue, [this]() {
				m_playbackQueue->previous();
				m_audioPlayer->play();
		});

		connect(m_playbackControlsWidget, &PlayerControls::changeShuffleState, m_playbackQueue, &PlaybackQueue::shuffle);
		connect(m_playbackControlsWidget, &PlayerControls::changeLoopedState, m_playbackQueue, [this]() {
				m_playbackQueue->setPlaybackMode(PlaybackQueue::CurrentItemInLoop);
		});

		connect(m_playbackQueue, &PlaybackQueue::currentMediaChanged, m_audioPlayer, &QMediaPlayer::setSource);
		connect(m_playbackQueue, &PlaybackQueue::currentIndexChanged, this, &MainWindow::selectRow);

		m_mainGridLayout->setSpacing(5);

		m_middleTreeView->setModel(m_middleModel);
		m_middleTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_middleTreeView->header()->setDefaultAlignment(Qt::AlignCenter);
		m_playerMainTreeView->header()->setDefaultAlignment(Qt::AlignCenter);

		m_coverLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		m_coverLabel->setPixmap(m_coverImage);
		m_coverLabel->setScaledContents(true);

		m_mainGridLayout->addWidget(m_playbackControlsWidget, 0, 0, 1, 33);
		m_mainGridLayout->addWidget(m_sideBarWidget, 1, 0, 13, 1);
		m_mainGridLayout->addWidget(m_middleTreeView, 1, 1, 9, 5);
		m_mainGridLayout->addWidget(m_coverLabel, 10, 1, 4, 5);
		m_mainGridLayout->addWidget(m_playerMainTreeView, 1, 6, 13, 27);

		connect(m_playerMainTreeView, &QTreeView::clicked, this, &MainWindow::rowClicked);
		connect(m_middleTreeView, &QTreeView::clicked, this, &MainWindow::onMiddleViewClicked);

		m_centralWidget->setLayout(m_mainGridLayout);
		setCentralWidget(m_centralWidget);

		m_middleTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(m_middleTreeView, &QTreeView::customContextMenuRequested, this, &MainWindow::onPlaylistContextMenu);

		m_playerMainTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(m_playerMainTreeView, &QTreeView::customContextMenuRequested, this, &MainWindow::onSongContextMenu);
}

void MainWindow::setupPlayerModel() {
		ZoneScoped;

		if (m_playerMainTreeView->model()) {
				delete m_playerMainTreeView->model();
		}
		const QVariantList COLUMNS_NAME{"Number", "Title", "Album", "Author", "Duration", "Year", "Bitrate", "File Size", "Path"};

		m_playerModel = new TreeModel(m_tracks, COLUMNS_NAME, this);

		m_playerMainTreeView->setModel(m_playerModel);

		for (int32_t c = 0; c < m_playerModel->columnCount(QModelIndex()); ++c) {
				m_playerMainTreeView->resizeColumnToContents(c);
		}

		m_playerMainTreeView->show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
		ZoneScoped;

		//if not maximized saves coordinates and size of window
		if (!isMaximized()) {
			const auto s =size();
			const auto p = pos();
			m_settings.setSettingsEntry("window_width", std::to_string(s.width()));
			m_settings.setSettingsEntry("window_height", std::to_string(s.height()));
			m_settings.setSettingsEntry("window_pos_x", std::to_string(p.x()));
			m_settings.setSettingsEntry("window_pos_y", std::to_string(p.y()));
		}
		
		//saves if maximized
		m_settings.getSettingsEntry("window_maximized", ?isMaximized() ? "1":"0");

		m_settings.saveSettings();
		QMainWindow::closeEvent(event);
}

void MainWindow::readWindowGeometrySettings() {
		ZoneScoped;

		const auto maxStr = m_settings.getSettingsEntry("window_maximized");
		const auto posXStr = m_settings.getSettingsEntry("window_pos_x");
		const auto posYStr = m_settings.getSettingsEntry("window_pos_y");
		const auto widthStr = m_settings.getSettingsEntry("window_width");
		const auto heightStr = m_settings.getSettingsEntry("window_height");

		QPoint posDefault = pos();
    	QSize  sizeDefault = size();

		if (!posXStr.empty() && !posYStr.empty()) {
        	posDefault.setX(std::stoi(posXStr));
        	posDefault.setY(std::stoi(posYStr));
    	}

		if (!widthStr.empty() && !heightStr.empty()) {
        	sizeDefault.setWidth(std::stoi(widthStr));
        	sizeDefault.setHeight(std::stoi(heightStr));
    	}
		
		move(posDefault);
    	resize(sizeDefault);

		if (!maxStr.empty()&&maxStr=="1") {
			showMaximized();
		}
}

void MainWindow::showLibrary() {
		ZoneScoped;

		m_currentViewMode = ViewMode::Library;
		if (m_middleModel) {
				m_middleModel->clear();
		}
		scanLibrary();

		setupPlayerModel();

		m_playbackQueue->setQueue(m_trackPaths);
}

void MainWindow::showPlaylists() {
		ZoneScoped;

		m_currentViewMode = ViewMode::Playlists;
		m_middleModel->clear();
		m_middleModel->setHorizontalHeaderLabels({"Playlists"});

		QDir dir = getPlaylistsDir();
		QStringList files = dir.entryList({"*.txt"}, QDir::Files);

		for (const QString &f : files) {
				QStandardItem *item = new QStandardItem(Icons::PLAYLIST, QFileInfo(f).baseName());
				item->setData(f);
				m_middleModel->appendRow(item);
		}
}

void MainWindow::showFavorite() {
}

void MainWindow::showAlbums() {
}

void MainWindow::showAuthors() {
}

void MainWindow::scanLibrary() {
		ZoneScoped;

		m_tracks.clear();

		QString pathStr = QString::fromStdString(m_settings.getSettingsEntry("libraryDirectory"));
		if (pathStr.isEmpty()) {
				pathStr = QDir::homePath() + "/Music";
		}
		QDir library{pathStr};

		if (!library.exists()) {
				return;
		}

		constexpr auto FLAGS = QDirListing::IteratorFlag::Recursive | QDirListing::IteratorFlag::FilesOnly;
		const QStringList AUDIO_FILE_FILTER = {"*.mp4", "*.mp3", "*.flac", "*.wav", "*.ogg", "*.opus", "*.m4a"}; // Wspierane typy plików

		for (const auto &file : QDirListing(library.path(), AUDIO_FILE_FILTER, FLAGS)) {
				m_trackPaths.append(file.absoluteFilePath());
		}

		if (m_trackPaths.isEmpty()) {
				return;
		}

		QFuture<Track> future = QtConcurrent::mapped(m_trackPaths, [this](const QUrl &filePath) -> Track {
				TagLib::FileRef fileRef{QFile::encodeName(filePath.toString()).constData()};

				return extractMetadata(filePath.toString(), fileRef);
		});
		future.waitForFinished();

		const auto results = future.results();
		m_tracks.reserve(results.size());

		for (const auto &track : results) {
				m_tracks.push_back(std::move(track));
		}
}

Track MainWindow::extractMetadata(const QString &filePath, const TagLib::FileRef &fileRef) {
		ZoneScoped;

		if (fileRef.isNull() || fileRef.tag() == nullptr) {
				return Track{};
		}

		if (filePath.isEmpty()) {
				return Track{};
		}

		const QFileInfo FILE_INFO{filePath};
		const TagLib::Tag *TAG{fileRef.tag()};
		const TagLib::AudioProperties *AUDIO_PROPERTIES{fileRef.audioProperties()};

		const uint32_t NUMBER{TAG->track()};
		const QString TITLE{QString::fromStdWString(TAG->title().toWString())};
		const QString ALBUM{QString::fromStdWString(TAG->album().toWString())};
		const QString ARTIST{QString::fromStdWString(TAG->artist().toWString())};
		const uint32_t YEAR{TAG->year()};

		const int32_t DURATION_IN_SECONDS{AUDIO_PROPERTIES->lengthInSeconds()};
		const int32_t BITRATE{AUDIO_PROPERTIES->bitrate()};

		const QString COVER_ART_PATH{findCoverArt(FILE_INFO)};

		const QTime DURATION{0, DURATION_IN_SECONDS / 60, DURATION_IN_SECONDS % 60};

		constexpr float MIB = 1024.0F * 1024.0F;
		const float FILE_SIZE = static_cast<float>(FILE_INFO.size()) / MIB;

		Track track{};

		track.number = NUMBER;
		track.title = TITLE;
		track.album = ALBUM;
		track.artist = ARTIST;
		track.duration = DURATION.toString("mm:ss");
		track.year = YEAR;
		track.bitrate = QString::number(BITRATE) + " kbps";
		track.fileSize = QString::number(FILE_SIZE, 'f', 1) + " MiB";
		track.coverArtPath = COVER_ART_PATH;
		track.path = QDir::toNativeSeparators(filePath);

		return track;
}

void MainWindow::selectRow(int32_t currentRow) const {
		ZoneScoped;

		const QModelIndex CURRENT_MODEL_INDEX = m_playerModel->index(currentRow, 0, QModelIndex());

		m_playerMainTreeView->setCurrentIndex(CURRENT_MODEL_INDEX);
		m_playerMainTreeView->selectionModel()->select(CURRENT_MODEL_INDEX, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
		m_playerMainTreeView->scrollTo(CURRENT_MODEL_INDEX);
}

void MainWindow::rowClicked(const QModelIndex &current) {
		ZoneScoped;

		int32_t currentRow = current.row();

		m_playbackQueue->setCurrentIndex(currentRow);

		m_coverImage = m_tracks.at(currentRow).coverArtPath;
		m_coverLabel->setPixmap(m_coverImage);
}

QString MainWindow::findCoverArt(const QFileInfo &fileInfo) {
		ZoneScoped;

		const QString FILE_PATH{fileInfo.absolutePath()};

		constexpr auto FLAGS = QDirListing::IteratorFlag::Default;

		const QStringList COVER_FILES = {"cover.jpg", "cover.jpeg", "cover.png", "cover.webp"}; // Wspierane typy plików dla grafiki okładki

		for (const auto &file : QDirListing(FILE_PATH, COVER_FILES, FLAGS)) {
				return file.absoluteFilePath();
		}

		return nullptr;
}

QString MainWindow::getPlaylistsDir() {
		ZoneScoped;

		m_settings.loadSettings();
		std::string libPathStd = m_settings.getSettingsEntry("libraryDirectory");
		QString libPath = QString::fromStdString(libPathStd);
		if (libPath.isEmpty()) {
				libPath = QCoreApplication::applicationDirPath();
		}

		QString path = libPath + "/playlists";

		QDir dir(path);
		if (!dir.exists()) {
				dir.mkpath(".");
		}

		return path;
}

void MainWindow::onPlaylistContextMenu(const QPoint &pos) {
		ZoneScoped;

		if (m_currentViewMode != ViewMode::Playlists)
				return;

		QMenu menu(this);
		QAction *newAction = menu.addAction("Nowa Playlista");

		QAction *selected = menu.exec(m_middleTreeView->mapToGlobal(pos));
		if (selected == newAction) {
				createNewPlaylist();
		}
}

void MainWindow::createNewPlaylist() {
		ZoneScoped;

		bool ok;
		QString name = QInputDialog::getText(this, "Nowa Playlista", "Podaj nazwę:", QLineEdit::Normal, "", &ok);

		if (ok && !name.isEmpty()) {
				QString filePath = getPlaylistsDir() + "/" + name + ".txt";
				QFile file(filePath);
				if (file.open(QIODevice::WriteOnly)) {
						file.close();
						showPlaylists();
				}
		}
}

void MainWindow::onMiddleViewClicked(const QModelIndex &index) {
		ZoneScoped;

		if (m_currentViewMode == ViewMode::Playlists) {

				QString filename = m_middleModel->itemFromIndex(index)->data().toString();
				loadPlaylistContent(filename);
		}
}

void MainWindow::loadPlaylistContent(const QString &filename) {
		ZoneScoped;

		m_tracks.clear();

		QFile file(getPlaylistsDir() + "/" + filename);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
				QTextStream in(&file);
				while (!in.atEnd()) {
						QString line = in.readLine();
						if (!line.isEmpty() && QFile::exists(line)) {

								const TagLib::FileRef FILE_REF{QFile::encodeName(line).constData()};
								extractMetadata(line, FILE_REF);
						}
				}
				file.close();
		}

		setupPlayerModel();
}

void MainWindow::onSongContextMenu(const QPoint &pos) {
		ZoneScoped;

		QModelIndex index = m_playerMainTreeView->indexAt(pos);
		if (!index.isValid())
				return;

		QString filePath = TreeModel::dataAtColumn(index, Qt::DisplayRole, 8).toString();

		QMenu menu(this);
		QMenu *subMenu = menu.addMenu("Dodaj do playlisty");

		QDir dir = getPlaylistsDir();
		QStringList playlists = dir.entryList({"*.txt"}, QDir::Files);

		if (playlists.isEmpty()) {
				subMenu->addAction("Brak playlist")->setEnabled(false);
		} else {
				for (const QString &pl : playlists) {
						QAction *act = subMenu->addAction(QFileInfo(pl).baseName());

						connect(act, &QAction::triggered, this, [this, pl, filePath]() {
								QFile file(getPlaylistsDir() + "/" + pl);
								if (file.open(QIODevice::Append | QIODevice::Text)) {
										QTextStream out(&file);
										out << filePath << "\n";
										file.close();
								}
						});
				}
		}

		menu.exec(m_playerMainTreeView->mapToGlobal(pos));
}

void MainWindow::addSongToPlaylist(const QString &playlistName) {
		ZoneScoped;

		QModelIndex index = m_playerMainTreeView->currentIndex();
		if (!index.isValid())
				return;

		QString filePath = TreeModel::dataAtColumn(index, Qt::DisplayRole, 8).toString();
		if (filePath.isEmpty())
				return;

		QString fileName = playlistName;
		if (!fileName.endsWith(".txt")) {
				fileName += ".txt";
		}

		QFile file(getPlaylistsDir() + "/" + fileName);
		if (file.open(QIODevice::Append | QIODevice::Text)) {
				QTextStream out(&file);
				out << filePath << "\n";
				file.close();
		}
}
