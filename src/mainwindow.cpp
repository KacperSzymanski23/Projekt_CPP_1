#include "mainwindow.hpp"
#include "icons.hpp"
#include "logs.hpp"
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
	, m_settings(Settings::getInstance())
	, m_coverLabel(new QLabel(this))
	, m_middleTreeView(new QTreeView(this))
	, m_playerMainTreeView(new QTreeView(this))
	, m_centralLayout(new QVBoxLayout(m_centralWidget))
	, m_lowerHorizontalLayout(new QHBoxLayout())
	, m_middleVerticalLayout(new QVBoxLayout())
	, m_libraryModel(nullptr)
	, m_audioPlayer(new QMediaPlayer(this))
	, m_audioOutput(new QAudioOutput(this))
	, m_middleModel(new QStandardItemModel(this)) {
		ZoneScoped;

		QString libraryPath = QString::fromStdString(m_settings.getSettingsEntry("libraryDirectory"));
		m_library = Library{libraryPath};

		m_library.scanLibraryPath();

		m_audioPlayer->setAudioOutput(m_audioOutput);

		m_playbackControlsWidget->setVolume(m_audioOutput->volume());
		m_playbackControlsWidget->setMuted(m_playbackControlsWidget->isMuted());

		m_middleTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
		m_playerMainTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

		setupConnections();
		setupLayout();

		showLibrary();

		readWindowGeometrySettings();
}

void MainWindow::setupConnections() {
		ZoneScoped;

		// Połączenia pomiedzy sygnałami z sidebara i slotami z mainwindow
		connect(m_sideBarWidget, &SideBar::showLibraryClicked, this, &MainWindow::showLibrary);
		connect(m_sideBarWidget, &SideBar::showPlaylistsClicked, this, &MainWindow::showPlaylists);
		connect(m_sideBarWidget, &SideBar::showAlbumsClicked, this, &MainWindow::showAlbums);
		connect(m_sideBarWidget, &SideBar::showFavoriteClicked, this, &MainWindow::showFavorite);

		// Połączenia odpowiadające za pauzowanie i odpawzowywanie odtwarzania
		connect(m_playbackControlsWidget, &PlayerControls::pause, m_audioPlayer, &QMediaPlayer::pause);
		connect(m_playbackControlsWidget, &PlayerControls::play, m_audioPlayer, &QMediaPlayer::play);

		// Połączenia odpowiadające za akualizowanie informacji długości obecnie odtwarzanego utworu i postepu odtwarzania
		connect(m_audioPlayer, &QMediaPlayer::durationChanged, m_playbackControlsWidget, &PlayerControls::setTrackDuration);
		connect(m_audioPlayer, &QMediaPlayer::positionChanged, m_playbackControlsWidget, &PlayerControls::setTrackProgress);

		// Połączenia odpowiadające za zmiane progresu odtwarzania w QMediaPlayer
		connect(m_playbackControlsWidget, &PlayerControls::changeProgress, m_audioPlayer, &QMediaPlayer::setPosition);

		// Połączenia odpowiadające za zmianę głośności i wyciszanie odtwarzania w QAudioOutput
		connect(m_playbackControlsWidget, &PlayerControls::changeVolume, m_audioOutput, &QAudioOutput::setVolume);
		connect(m_playbackControlsWidget, &PlayerControls::changeMuteState, m_audioOutput, &QAudioOutput::setMuted);

		// Połączenia odpowiadające za aktualizowanie głośności i stanu wyciszania PlayerControls
		connect(m_audioOutput, &QAudioOutput::volumeChanged, m_playbackControlsWidget, &PlayerControls::setVolume);
		connect(m_audioOutput, &QAudioOutput::mutedChanged, m_playbackControlsWidget, &PlayerControls::setMuted);

		// Połączenie odpowiadające za aktualizowanie stanu odtwarzania w PlayerControls
		connect(m_audioPlayer, &QMediaPlayer::playbackStateChanged, m_playbackControlsWidget, [this](QMediaPlayer::PlaybackState arg) {
				m_playbackControlsWidget->setPlayerState(arg);
		});

		// Połączenie odpowiadające za aktualizowanie stanu "Favorite" PlayerControls
		connect(m_playbackControlsWidget, &PlayerControls::changeFavoriteState, this, [this](bool arg) {
				if (arg) {
						addSongToPlaylist("Favorite.txt");
				} else {
						removeSongFromPlaylist("Favorite.txt", m_playbackQueue->currentMedia());
				}
		});

		// Połączenie odpowiadające za aktualizowanie m_library i modeli w przypadku zmiany ustawień programu
		connect(m_sideBarWidget, &SideBar::settingsChanged, this, [this]() {
				m_settings.loadSettings();

				QString libraryPath = QString::fromStdString(m_settings.getSettingsEntry("libraryDirectory"));

				m_library.setLibraryPath(libraryPath);
				m_library.scanLibraryPath();

				showLibrary();
		});

		// Połączenia odpowiadające za odtwarzanie kolejnego/poprzedniego utworu
		connect(m_playbackControlsWidget, &PlayerControls::next, m_playbackQueue, [this]() {
				m_playbackQueue->next();
				m_audioPlayer->play();
		});
		connect(m_playbackControlsWidget, &PlayerControls::previous, m_playbackQueue, [this]() {
				m_playbackQueue->previous();
				m_audioPlayer->play();
		});

		// Połączenia odpowiadające za aktualizowanie stanu "Shuffle" oraz "Loop" w PlaybackQueue
		connect(m_playbackControlsWidget, &PlayerControls::changeShuffleState, m_playbackQueue, &PlaybackQueue::shuffle);
		connect(m_playbackControlsWidget, &PlayerControls::changeLoopedState, m_playbackQueue, [this]() {
				m_playbackQueue->setPlaybackMode(PlaybackQueue::CurrentItemInLoop);
		});

		// Połączenia odpowiadające za aktualizowanie ścierzki obecnie odtwarzanego utworu w QMediaPlayer
		connect(m_playbackQueue, &PlaybackQueue::currentMediaChanged, m_audioPlayer, &QMediaPlayer::setSource);
		connect(m_playbackQueue, &PlaybackQueue::currentIndexChanged, this, &MainWindow::selectRow);

		// Połączenia odpowiadające kliknięcia na elementy w m_middleTreeView i m_playerMainTreeView
		connect(m_playerMainTreeView, &QTreeView::clicked, this, &MainWindow::rowClicked);
		connect(m_middleTreeView, &QTreeView::clicked, this, &MainWindow::onMiddleViewClicked);

		// Połączenie odpowiadające za otwieranie menu kontekstowego w m_middleTreeView i m_playerMainTreeView
		connect(m_middleTreeView, &QTreeView::customContextMenuRequested, this, &MainWindow::onPlaylistContextMenu);
		connect(m_playerMainTreeView, &QTreeView::customContextMenuRequested, this, &MainWindow::onSongContextMenu);
}

void MainWindow::setupLayout() {
		ZoneScoped;

		// Ustawienie skalowania okładki albumu
		m_coverLabel->setPixmap(m_coverImage);
		m_coverLabel->setScaledContents(true);
		m_coverLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_coverLabel->setMaximumSize(300, 300);

		// Ustawienie marginesów i odstępów miedzy elementami w m_lowerHorizontalLayout
		m_lowerHorizontalLayout->setSpacing(5);
		m_lowerHorizontalLayout->setContentsMargins(0, 0, 0, 0);

		m_lowerHorizontalLayout->addWidget(m_sideBarWidget);

		// Dodawanie elementów do m_middleVerticalLayout
		m_middleVerticalLayout->addWidget(m_middleTreeView, 3);
		m_middleVerticalLayout->addWidget(m_coverLabel, 1);

		// Dodanie zaganieżdzonego układu do m_lowerHorizontalLayout
		m_lowerHorizontalLayout->addLayout(m_middleVerticalLayout);

		m_lowerHorizontalLayout->addWidget(m_playerMainTreeView, 5);

		// Ustawienie marginesów i odstępów miedzy elementami w m_centralLayout
		m_centralLayout->setContentsMargins(0, 0, 10, 10);
		m_centralLayout->setSpacing(5);

		m_centralLayout->addWidget(m_playbackControlsWidget);

		m_centralLayout->addLayout(m_lowerHorizontalLayout);

		// Ustawienie stylów dla m_playerMainTreeView i m_middleTreeView
		m_middleTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_middleTreeView->header()->setDefaultAlignment(Qt::AlignCenter);
		m_playerMainTreeView->header()->setDefaultAlignment(Qt::AlignCenter);

		setCentralWidget(m_centralWidget);
}

void MainWindow::setupPlayerModel(const QList<Library::TrackMetadata> &trackMetadatas) {
		ZoneScoped;

		const QVariantList COLUMNS_NAME{"Number", "Title", "Album", "Author", "Duration", "Year", "Bitrate", "File Size"};

		// Jeżeli model jest pusty to tworzymy nowy i przypisujemy go do m_playerMainTreeView
		if (m_playerMainTreeView->model() == nullptr) {
				m_playerModel = new PlayerTreeModel(trackMetadatas, COLUMNS_NAME, this);
				m_playerMainTreeView->setModel(m_playerModel);
		} else {
				// Jeżeli model nie jest pusty to aktualizujemy jego dane
				m_playerModel->updateModelData(trackMetadatas);
		}

		// Ustawienie rozmiaru poszczególnych kolumn w m_playerMainTreeView
		auto *header = m_playerMainTreeView->header();

		header->setSectionResizeMode(0, QHeaderView::ResizeToContents); // Num
		header->setSectionResizeMode(1, QHeaderView::Stretch);          // Title
		header->setSectionResizeMode(2, QHeaderView::Stretch);          // Album
		header->setSectionResizeMode(3, QHeaderView::Stretch);          // Author
		header->setSectionResizeMode(4, QHeaderView::ResizeToContents); // Duration
		header->setSectionResizeMode(5, QHeaderView::ResizeToContents); // Year
		header->setSectionResizeMode(6, QHeaderView::ResizeToContents); // Bitrate
		header->setSectionResizeMode(7, QHeaderView::Stretch);          // File Size

		m_playerMainTreeView->show();
}

void MainWindow::loadPlaylistToList(const QString &playlistName, QList<QString> &tracksPaths) {
		ZoneScoped;

		if (playlistName.isEmpty()) {
				logCreate("Playlist name is empty");
				return;
		}

		QString fileName = playlistName;
		if (!fileName.endsWith(".txt")) {
				fileName += ".txt";
		}

		QFile file(getPlaylistsDir() + "/" + fileName);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
				QTextStream in(&file);
				while (!in.atEnd()) {
						QString line = in.readLine();
						if (!line.isEmpty() && QFile::exists(line)) {
								tracksPaths.append(line);
						}
				}
				file.close();
		}
}

void MainWindow::closeEvent(QCloseEvent *event) {
		ZoneScoped;

		// if not maximized saves coordinates and size of window
		if (!isMaximized()) {
				const auto SIZE = size();
				const auto POSITION = pos();
				m_settings.setSettingsEntry("window_width", std::to_string(SIZE.width()));
				m_settings.setSettingsEntry("window_height", std::to_string(SIZE.height()));
				m_settings.setSettingsEntry("window_pos_x", std::to_string(POSITION.x()));
				m_settings.setSettingsEntry("window_pos_y", std::to_string(POSITION.y()));
		}

		// saves if maximized
		m_settings.setSettingsEntry("window_maximized", isMaximized() ? "1" : "0");

		m_settings.saveSettings();
		QMainWindow::closeEvent(event);
}

void MainWindow::readWindowGeometrySettings() {
		ZoneScoped;

		const auto MAX_STR = m_settings.getSettingsEntry("window_maximized");
		const auto POS_X_STR = m_settings.getSettingsEntry("window_pos_x");
		const auto POS_Y_STR = m_settings.getSettingsEntry("window_pos_y");
		const auto WIDTH_STR = m_settings.getSettingsEntry("window_width");
		const auto HEIGHT_STR = m_settings.getSettingsEntry("window_height");

		QPoint posDefault = pos();
		QSize sizeDefault = size();

		if (!POS_X_STR.empty() && !POS_Y_STR.empty()) {
				posDefault.setX(std::stoi(POS_X_STR));
				posDefault.setY(std::stoi(POS_Y_STR));
		}

		if (!WIDTH_STR.empty() && !HEIGHT_STR.empty()) {
				sizeDefault.setWidth(std::stoi(WIDTH_STR));
				sizeDefault.setHeight(std::stoi(HEIGHT_STR));
		}

		move(posDefault);
		resize(sizeDefault);

		if (!MAX_STR.empty() && MAX_STR == "1") {
				showMaximized();
		}
}

void MainWindow::showLibrary() {
		ZoneScoped;

		if (m_libraryModel == nullptr) {
				m_libraryModel = new MiddleTreeModel(m_library.getArtistList(), "Library", this);
		} else {
				m_libraryModel->updateModelData(m_library.getArtistList());
		}

		m_currentViewMode = ViewMode::Library;

		if (m_middleTreeView->model() != m_libraryModel) {
				m_middleTreeView->setModel(m_libraryModel);
		}

		m_middleTreeView->show();
}

void MainWindow::showPlaylists() {
		ZoneScoped;

		m_currentViewMode = ViewMode::Playlists;
		if (m_middleModel != nullptr) {
				m_middleModel->clear();
		}

		if (m_middleModel == nullptr) {
				logCreate("m_middleModel is nullptr");
				return;
		}

		m_middleModel->setHorizontalHeaderLabels({"Playlists"});

		QDir dir = getPlaylistsDir();
		QStringList files = dir.entryList({"*.txt"}, QDir::Files);

		for (const QString &f : files) {
				QStandardItem *item = new QStandardItem(Icons::PLAYLIST, QFileInfo(f).baseName());
				item->setData(f);
				m_middleModel->appendRow(item);
		}

		m_middleTreeView->setModel(m_middleModel);
}

void MainWindow::showFavorite() {
		createNewPlaylist("Favorite");
		showPlaylists();
		loadPlaylistContent("Favorite");
}

void MainWindow::showAlbums() {
		ZoneScoped;

		m_currentViewMode = ViewMode::Albums;
		if (m_middleModel != nullptr) {
				m_middleModel->clear();
		}

		if (m_middleModel == nullptr) {
				logCreate("m_middleModel is nullptr");
				return;
		}

		m_middleModel->setHorizontalHeaderLabels({"Albums"});

		for (const auto &artist : m_library.getArtistList()) {
				for (const auto &album : artist.getItems()) {
						QStandardItem *item = new QStandardItem(Icons::ALBUMS, album.getName());
						m_middleModel->appendRow(item);
				}
		}

		m_middleTreeView->setModel(m_middleModel);
}

void MainWindow::selectRow(int32_t currentRow) const {
		ZoneScoped;

		if (currentRow == -1) {
				logCreate("Current row is -1, invalid index");
				return;
		}

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

		if (m_currentViewMode != ViewMode::Playlists) {
				return;
		}

		QMenu menu(this);
		QAction *newAction = menu.addAction("Nowa Playlista");

		QAction *selected = menu.exec(m_middleTreeView->mapToGlobal(pos));
		if (selected == newAction) {
				createNewPlaylistDialog();
		}
}

void MainWindow::createNewPlaylist(const QString &playlistName) {
		ZoneScoped;

		if (playlistName.isEmpty()) {
				logCreate("Playlist name is empty");
				return;
		}

		QString filePath = getPlaylistsDir() + "/" + playlistName + ".txt";
		QFile file(filePath);
		if (file.open(QIODevice::WriteOnly)) {
				file.close();
				showPlaylists();
		}
}

void MainWindow::createNewPlaylistDialog() {
		ZoneScoped;

		bool ok = false;
		QString name = QInputDialog::getText(this, "Nowa Playlista", "Podaj nazwę:", QLineEdit::Normal, "", &ok);

		if (ok) {
				createNewPlaylist(name);
		}
}

void MainWindow::onMiddleViewClicked(const QModelIndex &index) {
		ZoneScoped;

		const qsizetype ROW = index.row();
		const qsizetype PARENT_ROW = index.parent().row();

		if (m_currentViewMode == ViewMode::Playlists) {
				QString filename = m_middleModel->itemFromIndex(index)->data().toString();
				loadPlaylistContent(filename);

		} else if (m_currentViewMode == ViewMode::Library) {
				if (PARENT_ROW == -1) {
						return;
				}

				const Library::Artist ARTIST{m_library.getArtist(PARENT_ROW)};
				const Library::Album ALBUM{ARTIST.getItem(ROW)};
				const QList PATHS{ALBUM.getTracksPathsList()};
				const QList TRACKS{ALBUM.getItems()};

				m_coverImage = ALBUM.getCoverArtPath();
				m_coverLabel->setPixmap(m_coverImage);

				m_playbackQueue->setQueue(PATHS);
				setupPlayerModel(TRACKS);

		} else if (m_currentViewMode == ViewMode::Albums) {
				const Library::Album ALBUM{m_library.getAlbum(ROW)};
				const QList PATHS{ALBUM.getTracksPathsList()};
				const QList TRACKS{ALBUM.getItems()};

				m_coverImage = ALBUM.getCoverArtPath();
				m_coverLabel->setPixmap(m_coverImage);

				m_playbackQueue->setQueue(PATHS);
				setupPlayerModel(TRACKS);
		}
}

void MainWindow::loadPlaylistContent(const QString &playlistName) {
		ZoneScoped;

		QList<Library::TrackMetadata> trackList{};
		QList<QString> pathList{};

		QFile file(getPlaylistsDir() + "/" + playlistName);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
				QTextStream in(&file);
				while (!in.atEnd()) {
						QString line = in.readLine();
						if (!line.isEmpty() && QFile::exists(line)) {
								auto [metadata, path] = Library::extractMetadata(line);

								trackList.append(metadata);
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
		if (!index.isValid()) {
				logCreate("Invalid index at position: " + std::to_string(pos.x()) + ", " + std::to_string(pos.y()));
				return;
		}

		QString filePath = m_playbackQueue->currentMedia();

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
		if (!index.isValid()) {
				return;
		}

		QString filePath = TreeModel::data(index, Qt::DisplayRole, 8).toString();
		if (filePath.isEmpty()) {
				return;
		}

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

void MainWindow::removeSongFromPlaylist(const QString &playlistName, const QString &filePath) {
		ZoneScoped;

		QList<QString> paths{};
		loadPlaylistToList(playlistName, paths);

		if (paths.removeOne(filePath)) {
				QFile file(getPlaylistsDir() + "/" + playlistName);
				if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
						QTextStream out(&file);
						for (const QString &path : paths) {
								out << path << "\n";
						}
						file.close();
				}
		}
}