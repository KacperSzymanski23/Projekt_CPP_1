#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "library.hpp"
#include "logs.hpp"
#include "middletreemodel.hpp"
#include "playbackqueue.hpp"
#include "playercontrols.hpp"
#include "playertreemodel.hpp"
#include "settings.hpp"
#include "sidebar.h"
#include "treemodel.hpp"
// Qt
#include <QAudioOutput>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QSettings>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>
// TabLib
#include <taglib/fileref.h>

class MainWindow : public QMainWindow {
		Q_OBJECT

	  public:
		MainWindow();

	  private:
		void setupPlayerModel(const QList<Library::TrackMetadata> &trackMetadatas); // Tworzy model elemetów dla m_playerMainTreeView

		PlaybackQueue *m_playbackQueue;

		QWidget *m_centralWidget; // Centraly widget okna
		SideBar *m_sideBarWidget; // Widget dla bocznego paska narzędzi

		PlayerControls *m_playbackControlsWidget; // Widget umożliwiający sterowanei odtwarzaniem

		Settings m_settings; // Ustawienia
		Library m_library;

		QLabel *m_coverLabel; // Etykieta wyświetlająca okładkę dla każdej ścieżki

		QToolBar *m_sideToolBar; // Pasek narzędzi

		QTreeView *m_middleTreeView;     // QTreeView zawierający wybrane przez użytkownika elementy np. albumy, autorów itd.
		QTreeView *m_playerMainTreeView; // QTreeView zawierające wszystkie ścieżki dźwiękowe w albumnie lub wszystkie albumy autorstwa
		                                 // danego autora

		QVBoxLayout *m_sideBarLayout;  // Układ elementów GUI dla paska bocznego
		QGridLayout *m_mainGridLayout; // Układ elementów GUI dla widgetu m_centralWidget
		QVBoxLayout *m_middleListLayout;

		MiddleTreeModel *m_libraryModel;
		PlayerTreeModel *m_playerModel; // Model dla m_playerMainTreeView odpowiadający za liczbę i nazwy oraz zarządzanie kolumn i informacjami w
		                                // QTreeView
		QMediaPlayer *m_audioPlayer;    // Objekt klasy umożliwiającej odtwarzanie i kontrolowanie odtwarzania plików zawierających audio
		QAudioOutput *m_audioOutput;    // Objekt klasy reprezentującej kanały wyjśća audio dla m_audioPlayer

		QPixmap m_coverImage; // Okładka dla obecnie wybranej ścieżki dźwiękowej

		QStandardItemModel *m_middleModel; // Model dla środkowego panelu (listy playlist)

		QString m_currentPlaylistName; // przechowuje nazwe otwartej playlisty

		enum class ViewMode { Library = 0, Albums = 1, Playlists = 2, Favorite = 3, None = 4 };
		ViewMode m_currentViewMode = ViewMode::None;
		QString getPlaylistsDir();                             // Pomocnicza funkcja do folderu
		void loadPlaylistContent(const QString &playlistName); // Wczytuje zawartość playlisty

	  private slots:

		void showLibrary();   // Wyświetla wszystkich autorów i wraz z albumami w m_middleTreeView
		void showAlbums();    // Wyświetla wszystkie albumy w m_middleTreeView
		void showPlaylists(); // Wyświetla wszystkie playlisty w m_middleTreeView
		void showFavorite();  // Wyświetla wszystkie ścieżki dźwiękowe z playlisty fovorite w m_middleTreeView

		void closeEvent(QCloseEvent *event) override; // Funkcja slot obsługująca zamykanie okna
		void readWindowGeometrySettings();            // Funkcja slot zapisująca stan okna

		void selectRow(int32_t currentRow) const;
		void rowClicked(const QModelIndex &current); // Pobiera dane z piosenki z kliniętego przez użytkownika wiersza

		void onMiddleViewClicked(const QModelIndex &index); // Kliknięcie w playlistę
		void onPlaylistContextMenu(const QPoint &pos);      // Menu: Nowa playlista
		void onSongContextMenu(const QPoint &pos);          // Menu: Dodaj do playlisty

		void createNewPlaylist(const QString &playlistName);                            // Logika tworzenia pliku
		void createNewPlaylistDialog();                            // Okno dialogowe tworzenia pliku
		void addSongToPlaylist(const QString &playlistName); // Dodawanie utworu
};

#endif /* MAINWINDOW_HPP */
