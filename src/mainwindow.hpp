#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "track.hpp"
#include "treeitem.hpp"
#include "treemodel.hpp"
// Qt
#include <QAction>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QSettings>
#include <QSlider>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
		Q_OBJECT

	  public:
		MainWindow();

	  private slots:
		void defaultAction(); // To remove

		void showLibrary();
		void showPlaylists();
		void showFavorite();
		void showAuthors();
		void showFiles();

		void closeEvent(QCloseEvent *event) override;
		void readWindowGeometrySettings();

	  private:
		void setupSideBar();
		void setupPlaybackControll();
		void setupPlayerModel();

		const QStringList AUDIO_FILE_FILTER = {"*.mp4", "*.mp3", "*.flac", "*.wav", "*.ogg", "*.opus", "*.m4a"};

		QSlider *m_playbackSlider;
		QSlider *m_volumeSlider;

		QLabel *m_timeLabel;
		QLabel *m_volumeLabel;
		QLabel *m_coverLabel;

		QWidget *m_centralWidget;
		QWidget *m_sideBarWidget;
		QWidget *m_playbackControllWidget;

		QTreeView *m_middleTreeView;
		QTreeView *m_playerMainTreeView;

		QVBoxLayout *m_sideBarLayout;
		QGridLayout *m_mainGridLayout;
		QGridLayout *m_playbackControllLayout;

		QPixmap m_coverImage;

		std::vector<Track> m_tracks;
};

#endif /* MAINWINDOW_HPP */