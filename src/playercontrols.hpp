#ifndef PLAYERCONTROLS_HPP
#define PLAYERCONTROLS_HPP

#include <QHBoxLayout>
#include <QLabel>
#include <QMediaPlayer>
#include <QSlider>
#include <QToolButton>
#include <QWidget>

class PlayerControls : public QWidget {
		Q_OBJECT

	  public:
		explicit PlayerControls(QWidget *parent = nullptr);

		[[nodiscard]] float getVolume() const; // Pobiera obecny poziom głośności
		[[nodiscard]] bool isMuted() const;    // Zwraca wartość stanu wydziszenia odtwarzacza

		void setPlayerState(QMediaPlayer::PlaybackState state); // Ustawia stan odtwarzania

	  private:
		void updateProgressLabel(int32_t progress) const; // Aktualizuje postęp odtwarzania w m_progressLabel
		void updateVolumeIcon(int32_t volume) const;      // Aktualizuje ikonę głośności w m_audioButton

		QSlider *m_progressSlider; // Slider pozwalający na sterowanie postępem odtwarzania
		QSlider *m_volumeSlider;   // Slider pozwalający na sterowanie poziomem głośności

		QLabel *m_progressLabel; // Etykieta wyświetlająca postęp odtwarzania

		// Przyciski pozwalające na sterowani odtwarzaniem
		QToolButton *m_loopButton;
		QToolButton *m_shuffleButton;
		QToolButton *m_previousButton;
		QToolButton *m_playPauseButton;
		QToolButton *m_nextButton;
		QToolButton *m_audioButton;

		QToolButton *m_favoriteButton; // Przycisk zapisujący piosenke do playlisty favorite

		QHBoxLayout *m_playbackControlsLayout; // Układ elemntów GUI dla kontolek odtwarzania

		QMediaPlayer::PlaybackState m_playerState = QMediaPlayer::StoppedState; // Stan odtwarzania

		int32_t m_progress{}; // Obecny postę odtwarzania
		int32_t m_duration{}; // Długość obecnie odtwarzanej piosenki

		// Flagi stanów dla sterowania odtwarzaniem
		bool m_playerMuted = false;
		bool m_playerShuffled = false;
		bool m_playerLooped = false;

		// Flaga zawierająca informaję czy ta piosenka jest w playliscie favorite
		bool m_playerFavorite = false;

	  public slots:
		// Sloty do aktualizacji stanu odtwarzacza
		void setVolume(float volume) const;
		void setMuted(bool muted);
		void setShuffle(bool shuffle);
		void setLoop(bool loop);
		void setFavorite(bool favorite);
		void setTrackDuration(int32_t duration);
		void setTrackProgress(int32_t progress);

	  private slots:
		// Sloty obsługujące kliknięcia przycisków
		void playPauseClicked();
		void muteClicked();
		void shuffleClicked();
		void loopClicked();
		void favoriteClicked();

		// Sloty obsługujące zmienę wartości paska postępu oraz paska głośności
		void volumeSliderValueChanged();
		void progressSliderMoved();

	  signals:
		// Sygnały emitowane przy zmianie stanu odtwarzacza
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