#ifndef PLAYERCONTROLS_HPP
#define PLAYERCONTROLS_HPP

#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>

class PlayerControls : public QWidget {
		Q_OBJECT

	  public:
		explicit PlayerControls(QWidget *parent = nullptr);
		~PlayerControls() override;

		float getVolume() const;
		bool isMuted() const;

	  private:
		void updateProgressLabel(int32_t progress);

		QSlider *m_progressSlider;
		QSlider *m_volumeSlider;

		QLabel *m_progressLabel;

		QToolButton *m_loopButton;
		QToolButton *m_shuffleButton;
		QToolButton *m_previousButton;
		QToolButton *m_playPauseButton;
		QToolButton *m_nextButton;
		QToolButton *m_audioButton;
		QToolButton *m_favoriteButton;

		QHBoxLayout *m_playbackControllLayout;

		int32_t m_progress;
		int32_t m_duration;

		bool m_playerMuted = false;
		bool m_playerShuffled = false;
		bool m_playerLooped = false;
		bool m_playerFavorite = false;
		bool m_playerState = true;

	  public slots:
		void setVolume(float volume);
		void setMuted(bool muted);
		void setShuffle(bool shuffle);
		void setLoop(bool loop);
		void setFavorite(bool loop);
		void setTrackDuration(const int32_t DURATION);
		void setTrackProgress(int32_t duration);

	  private slots:
		void playPauseClicked();
		void muteClicked();
		void shuffleClicked();
		void loopClicked();
		void favoriteClicked();
		void volumeSliderValueChanged();
		void progressSliderMoved();

	  signals:
		void play();
		void pause();
		void next();
		void previous();
		void changeVolume(float volume);
		void changeMuteState(bool mute);
		void changeShuffleState(bool shuffle);
		void changeLoopedState(bool looped);
		void changeFavoriteState(bool favorite);
		void changeProgress(int32_t progress);
};

#endif /* PLAYERCONTROLS_HPP */