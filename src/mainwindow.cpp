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


		QUrl libraryPath = QString::fromStdString(m_settings.getSettingsEntry("libraryDirectory"));
		m_library = Library{libraryPath};

		m_audioPlayer->setAudioOutput(m_audioOutput);

		m_playbackControlsWidget->setVolume(m_audioOutput->volume());
		m_playbackControlsWidget->setMuted(m_playbackControlsWidget->isMuted());

		connect(m_sideBarWidget, &SideBar::showLibraryClicked, this, &MainWindow::showLibrary);
		connect(m_sideBarWidget, &SideBar::showPlaylistsClicked, this, &MainWindow::showPlaylists);
		connect(m_sideBarWidget, &SideBar::showAlbumsClicked, this, &MainWindow::showAlbums);
		connect(m_sideBarWidget, &SideBar::showAuthorsClicked, this, &MainWindow::showAuthors);

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

		m_library.scanLibraryPath();

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

		readWindowGeometrySettings();
}

void MainWindow::setupPlayerModel(const QList<Library::TrackMetadata> &trackMetadatas) {
		ZoneScoped;

		if (m_playerMainTreeView->model()) {
				delete m_playerMainTreeView->model();
		}
		const QVariantList COLUMNS_NAME{"Number", "Title", "Album", "Author", "Duration", "Year", "Bitrate", "File Size"};

		m_playerModel = new TreeModel(trackMetadatas, COLUMNS_NAME, this);

		m_playerMainTreeView->setModel(m_playerModel);

		for (int32_t c = 0; c < m_playerModel->columnCount(QModelIndex()); ++c) {
				m_playerMainTreeView->resizeColumnToContents(c);
		}

		m_playerMainTreeView->show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
		ZoneScoped;

		// if not maximized saves coordinates and size of window
		if (!isMaximized()) {
				const auto s = size();
				const auto p = pos();
				m_settings.setSettingsEntry("window_width", std::to_string(s.width()));
				m_settings.setSettingsEntry("window_height", std::to_string(s.height()));
				m_settings.setSettingsEntry("window_pos_x", std::to_string(p.x()));
				m_settings.setSettingsEntry("window_pos_y", std::to_string(p.y()));
		}

		// saves if maximized
		m_settings.setSettingsEntry("window_maximized", isMaximized() ? "1" : "0");

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
		QSize sizeDefault = size();

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

		if (!maxStr.empty() && maxStr == "1") {
				showMaximized();
		}
}

void MainWindow::showLibrary() {
		ZoneScoped;

		m_currentViewMode = ViewMode::Library;
		if (m_middleModel) {
				m_middleModel->clear();
		}
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
		ZoneScoped;

		m_currentViewMode = ViewMode::Albums;
		m_middleModel->clear();
		m_middleModel->setHorizontalHeaderLabels({"Albums"});

		for (const auto &artist : m_library.getArtistList()) {
				for (const auto &album : artist.getAlbumsList()) {
						QStandardItem *item = new QStandardItem(Icons::ALBUMS, album.getTitle());
						m_middleModel->appendRow(item);
				}
		}
}

void MainWindow::showAuthors() {
		ZoneScoped;

		m_currentViewMode = ViewMode::Artists;
		m_middleModel->clear();
		m_middleModel->setHorizontalHeaderLabels({"Artist"});

		for (const auto &artist : m_library.getArtistList()) {
				QStandardItem *item = new QStandardItem(Icons::AUTHORS, artist.getName());
				m_middleModel->appendRow(item);
		}
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
		if (m_currentViewMode == ViewMode::Albums) {
				const int32_t ROW = index.row();

				const Library::Album ALBUM{m_library.getAlbumsList().at(ROW)};
				const QList PATHS{ALBUM.getTracksPathsList()};
				const QList TRACKS{ALBUM.getTracksList()};

				m_coverImage = ALBUM.getCoverArtPath().toString();
				m_coverLabel->setPixmap(m_coverImage);

				m_playbackQueue->setQueue(PATHS);
				setupPlayerModel(TRACKS);
		}
}

void MainWindow::loadPlaylistContent(const QString &filename) {
		ZoneScoped;

		QList<Library::TrackMetadata> trackList{};
		QList<QUrl> pathList{};

		QFile file(getPlaylistsDir() + "/" + filename);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
				QTextStream in(&file);
				while (!in.atEnd()) {
						QString line = in.readLine();
						if (!line.isEmpty() && QFile::exists(line)) {

								const TagLib::FileRef FILE_REF{QFile::encodeName(line).constData()};

								trackList.append(Library::extractMetadata(line, FILE_REF));
								pathList.append(line);
						}
				}
				file.close();
		}

		m_playbackQueue->setQueue(pathList);
		setupPlayerModel(trackList);
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
