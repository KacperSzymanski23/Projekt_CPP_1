#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "playercontrols.hpp"
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
#include <QTreeView>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
		Q_OBJECT

	  public:
		MainWindow();

	  private:
		void setupSideBar();
		void setupPlayerModel();
		void scanLibrary();

		static QPixmap getCoverArt(const QString &path, const QString &extension);

		const QStringList AUDIO_FILE_FILTER = {"*.mp4", "*.mp3", "*.flac", "*.wav", "*.ogg", "*.opus", "*.m4a"};

		QWidget *m_centralWidget;
		QWidget *m_sideBarWidget;

		PlayerControls *m_playbackControlsWidget;

		QLabel *m_coverLabel;

		QToolBar *m_sideToolBar;

		QTreeView *m_middleTreeView;
		QTreeView *m_playerMainTreeView;

		QVBoxLayout *m_sideBarLayout;
		QGridLayout *m_mainGridLayout;
		QVBoxLayout *m_middleListLayout;

		TreeModel *m_playerModel;
		QMediaPlayer *m_audioPlayer;
		QAudioOutput *m_audioOutput;

		QPixmap m_coverImage;

		std::vector<Track> m_tracks;

	  private slots:
		void showLibrary();
		void showPlaylists();
		void showFavorite();
		void showAuthors();
		void showAlbums();

		void closeEvent(QCloseEvent *event) override;
		void readWindowGeometrySettings();

		void rowClicked(const QModelIndex &current);
};

#endif /* MAINWINDOW_HPP */