#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "playercontrols.hpp"
#include "settings.hpp"
#include "settingsdialog.hpp"
#include "track.hpp"
#include "treemodel.hpp"

// Qt
#include <QAction>
#include <QAudioOutput>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QSettings>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
		Q_OBJECT

	  public:
		MainWindow();

	  private:
		void setupSideBar();     // Tworzy pasek narzędzi po lewej stonie
		void setupPlayerModel(); // Tworzy model elemetów dla m_playerMainTreeView
		void scanLibrary();      // Skanuje wskananą ścieżkę w poszukiwaniu plików audio

		static QString findCoverArt(const QFileInfo &fileInfo); // Szuka okładki albumu w folderze

		QWidget *m_centralWidget; // Centraly widget okna
		QWidget *SideBar; // Widget dla bocznego paska narzędzi

		QToolButton *m_settingsButton; // Przycisk otwierający okno ustawień

		PlayerControls *m_playbackControlsWidget; // Widget umożliwiający sterowanei odtwarzaniem

		SettingsDialog *m_settingsDialog; // Okno ustawień
		Settings m_settings;              // Ustawienia

		QLabel *m_coverLabel; // Etykieta wyświetlająca okładkę dla każdej ścieżki

		QToolBar *m_sideToolBar; // Pasek narzędzi

		QTreeView *m_middleTreeView;     // QTreeView zawierający wybrane przez użytkownika elementy np. albumy, autorów itd.
		QTreeView *m_playerMainTreeView; // QTreeView zawierające wszystkie ścieżki dźwiękowe w albumnie lub wszystkie albumy autorstwa
		                                 // danego autora

		QVBoxLayout *m_sideBarLayout;  // Układ elementów GUI dla paska bocznego
		QGridLayout *m_mainGridLayout; // Układ elementów GUI dla widgetu m_centralWidget
		QVBoxLayout *m_middleListLayout;

		TreeModel *m_playerModel;    // Model dla m_playerMainTreeView odpowiadający za liczbę i nazwy oraz zarządzanie kolumn i informacjami w
		                             // QTreeView
		QMediaPlayer *m_audioPlayer; // Objekt klasy umożliwiającej odtwarzanie i kontrolowanie odtwarzania plików zawierających audio
		QAudioOutput *m_audioOutput; // Objekt klasy reprezentującej kanały wyjśća audio dla m_audioPlayer

		QPixmap m_coverImage; // Okładka dla obecnie wybranej ścieżki dźwiękowej

		std::vector<Track> m_tracks; // std::vector zawierający metadane i ścieżki plików audio

	  private slots:
		void showLibrary();   // Wyświetla wszystkie ścieżki dźwiękowe w m_middleTreeView
		void showPlaylists(); // Wyświetla wszystkie playlisty w m_middleTreeView
		void showFavorite();  // Wyświetla wszystkie ścieżki dźwiękowe z playlisty fovorite w m_middleTreeView
		void showAuthors();   // Wyświetla wszystkich autorów albumów w m_middleTreeView
		void showAlbums();    // Wyświetla wszystkie albumy w m_middleTreeView

		void closeEvent(QCloseEvent *event) override; // Funkcja slot obsługująca zamykanie okna
		void readWindowGeometrySettings();            // Funkcja slot zapisująca stan okna

		void rowClicked(const QModelIndex &current); // Pobiera dane z piosenki z kliniętego przez użytkownika wiersza
		void extractMetadata(const QString &filePath); // Funkcja pomocnicza do ekstrakcji danych
};

#endif /* MAINWINDOW_HPP */