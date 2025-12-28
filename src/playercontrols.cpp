#include "playercontrols.hpp"
#include "icons.hpp"
#include "logs.hpp"
// Qt
#include <QTime>
// Tracy
#include <tracy/Tracy.hpp>
// STD
#include <unistd.h>

PlayerControls::PlayerControls(QWidget *parent)
	: QWidget(parent)
	, m_progressSlider(new QSlider(this))
	, m_volumeSlider(new QSlider(this))
	, m_loopButton(new QToolButton(this))
	, m_shuffleButton(new QToolButton(this))
	, m_previousButton(new QToolButton(this))
	, m_playPauseButton(new QToolButton(this))
	, m_nextButton(new QToolButton(this))
	, m_audioButton(new QToolButton(this))
	, m_favoriteButton(new QToolButton(this))
	, m_playbackControlsLayout(new QHBoxLayout(this)) {
		ZoneScoped;

		m_playbackControlsLayout->setSpacing(0);

		constexpr QSize ICON_SIZE = QSize(24, 24);

		m_loopButton->setIcon(Icons::LOOP);
		m_loopButton->setIconSize(ICON_SIZE);
		m_loopButton->setToolTip(tr("Loop Playback"));
		m_loopButton->setCheckable(true);
		m_playbackControlsLayout->addWidget(m_loopButton);
		connect(m_loopButton, &QToolButton::clicked, this, &PlayerControls::loopClicked);

		m_shuffleButton->setIcon(Icons::SHUFFLE);
		m_shuffleButton->setIconSize(ICON_SIZE);
		m_shuffleButton->setToolTip(tr("Shuffle"));
		m_shuffleButton->setCheckable(true);
		m_playbackControlsLayout->addWidget(m_shuffleButton);
		connect(m_shuffleButton, &QToolButton::clicked, this, &PlayerControls::shuffleClicked);

		m_previousButton->setIcon(Icons::PREVIOUS);
		m_previousButton->setIconSize(ICON_SIZE);
		m_previousButton->setToolTip(tr("Previous Song"));
		m_playbackControlsLayout->addWidget(m_previousButton);
		connect(m_previousButton, &QToolButton::clicked, this, &PlayerControls::previous);

		m_playPauseButton->setIcon(Icons::PLAY);
		m_playPauseButton->setIconSize(ICON_SIZE);
		m_playPauseButton->setToolTip(tr("Start"));
		m_playbackControlsLayout->addWidget(m_playPauseButton);
		connect(m_playPauseButton, &QToolButton::clicked, this, &PlayerControls::playPauseClicked);

		m_nextButton->setIcon(Icons::NEXT);
		m_nextButton->setIconSize(ICON_SIZE);
		m_nextButton->setToolTip(tr("Next Song"));
		m_playbackControlsLayout->addWidget(m_nextButton);
		connect(m_nextButton, &QToolButton::clicked, this, &PlayerControls::next);

		m_progressSlider->setOrientation(Qt::Horizontal);
		m_playbackControlsLayout->addWidget(m_progressSlider);
		connect(m_progressSlider, &QSlider::sliderMoved, this, &PlayerControls::progressSliderMoved);
		connect(m_progressSlider, &QSlider::sliderPressed, this, &PlayerControls::progressSliderMoved);

		m_progressLabel = new QLabel(tr(" 00:00 / 00:00 "), this);
		m_playbackControlsLayout->addWidget(m_progressLabel);

		m_audioButton->setIcon(Icons::HIGH_VOLUME);
		m_audioButton->setIconSize(ICON_SIZE);
		m_audioButton->setToolTip(tr("Volume"));
		m_playbackControlsLayout->addWidget(m_audioButton);
		connect(m_audioButton, &QToolButton::clicked, this, &PlayerControls::muteClicked);

		m_volumeSlider->setOrientation(Qt::Horizontal);
		m_volumeSlider->setMaximumWidth(175);
		m_volumeSlider->setValue(100);
		m_playbackControlsLayout->addWidget(m_volumeSlider);

		connect(m_volumeSlider, &QSlider::valueChanged, this, &PlayerControls::volumeSliderValueChanged);

		m_favoriteButton->setIcon(Icons::FAVORITE);
		m_favoriteButton->setIconSize(ICON_SIZE);
		m_favoriteButton->setToolTip(tr("Add Favorite"));
		m_favoriteButton->setCheckable(true);
		m_playbackControlsLayout->addWidget(m_favoriteButton);

		connect(m_favoriteButton, &QToolButton::clicked, this, &PlayerControls::favoriteClicked);

		this->setLayout(m_playbackControlsLayout);
}

float PlayerControls::getVolume() const {
		return static_cast<float>(m_volumeSlider->value()) / 100.0F;
}

bool PlayerControls::isMuted() const {
		return m_playerMuted;
}

void PlayerControls::setPlayerState(QMediaPlayer::PlaybackState state) {
		ZoneScoped;

		if (state != m_playerState) {
				m_playerState = state;

				switch (state) {
						case QMediaPlayer::StoppedState:
								m_playPauseButton->setIcon(Icons::PLAY);
								break;
						case QMediaPlayer::PlayingState:
								m_playPauseButton->setIcon(Icons::PAUSE);
								break;
						case QMediaPlayer::PausedState:
								m_playPauseButton->setIcon(Icons::PLAY);
								break;
				}
		}
}

void PlayerControls::updateProgressLabel(int32_t progress) const {
		ZoneScoped;

		const QTime PROGRESS_TIME((progress / 3600) % 60, (progress / 60) % 60, progress % 60);
		const QTime DURATION_TIME((m_duration / 3600) % 60, (m_duration / 60) % 60, m_duration % 60);

		QString timeFormat{" mm:ss "};
		if (m_duration > 3600) {
				timeFormat = " hh:mm:ss ";
		}

		const QString PROGRESS_LABEL_TEXT = PROGRESS_TIME.toString(timeFormat) + "/" + DURATION_TIME.toString(timeFormat);

		m_progressLabel->setText(PROGRESS_LABEL_TEXT);
}

void PlayerControls::updateVolumeIcon(int32_t volume) const {
		ZoneScoped;

		if (volume > 75) {
				m_audioButton->setIcon(Icons::HIGH_VOLUME);
		} else if (volume > 25) {
				m_audioButton->setIcon(Icons::MEDIUM_VOLUME);
		} else if (volume > 0) {
				m_audioButton->setIcon(Icons::LOW_VOLUME);
		} else {
				m_audioButton->setIcon(Icons::MUTE);
		}
}

void PlayerControls::playPauseClicked() {
		ZoneScoped;

		if (m_playerState == QMediaPlayer::PlayingState) {
				m_playerState = QMediaPlayer::PausedState;
		} else {
				m_playerState = QMediaPlayer::PlayingState;
		}

		switch (m_playerState) {
				case QMediaPlayer::PlayingState:
						emit play();
						m_playPauseButton->setIcon(Icons::PAUSE);
						logCreate("Played");

						break;
				case QMediaPlayer::PausedState:
				case QMediaPlayer::StoppedState:
						emit pause();
						m_playPauseButton->setIcon(Icons::PLAY);
						logCreate("Paused");

						break;
		}
}

void PlayerControls::muteClicked() {
		emit changeMuteState(!m_playerMuted);
		if(isMuted()) {
			logCreate("Muted");
		}
		else {
			logCreate("Unmuted");
		}		
		
}

void PlayerControls::volumeSliderValueChanged() {
		const float VOLUME = static_cast<float>(m_volumeSlider->value());

		emit changeVolume(VOLUME / 100.0F);
		logCreate("Changed volume");
}

void PlayerControls::shuffleClicked() {
		emit changeShuffleState(!m_playerShuffled);
		logCreate("Shuffled");
}

void PlayerControls::loopClicked() {
		emit changeLoopedState(!m_playerLooped);
		logCreate("Looped");
}

void PlayerControls::favoriteClicked() {
		emit changeFavoriteState(!m_playerFavorite);
		setFavorite(!m_playerFavorite);
		logCreate("Set favourite");
}

void PlayerControls::progressSliderMoved() {
		emit changeProgress(m_progressSlider->value() * 1000);
		logCreate("Moved progress slider");
}

void PlayerControls::setVolume(float volume) const {
		const int VOLUME = static_cast<int>(volume * 100);

		updateVolumeIcon(VOLUME);

		m_volumeSlider->setValue(VOLUME);
}

void PlayerControls::setMuted(bool muted) {
		ZoneScoped;

		if (muted != m_playerMuted) {
				m_playerMuted = muted;

				if (muted) {
						m_audioButton->setIcon(Icons::MUTE);
				} else {
						updateVolumeIcon(m_volumeSlider->value());
				}
		}
}

void PlayerControls::setShuffle(bool shuffle) {
		if (shuffle != m_playerShuffled) {
				m_playerShuffled = shuffle;
		}
}

void PlayerControls::setLoop(bool loop) {
		if (loop != m_playerLooped) {
				m_playerLooped = loop;
		}
}

void PlayerControls::setFavorite(bool favorite) {
		if (favorite != m_playerFavorite) {
				m_playerFavorite = favorite;

				if (favorite) {
						m_favoriteButton->setIcon(Icons::FAVORITE);
				} else {
						m_favoriteButton->setIcon(Icons::NO_FAVORITE);
				}
		}
}

void PlayerControls::setTrackProgress(int32_t progress) {
		ZoneScoped;

		const int32_t CURRENT_PROGRESS{progress / 1000};

		m_progressSlider->setValue(CURRENT_PROGRESS);

		if (CURRENT_PROGRESS == m_duration) {
				emit next();
		}

		updateProgressLabel(CURRENT_PROGRESS);
}

void PlayerControls::setTrackDuration(int32_t duration) {
		m_duration = duration / 1000;
		m_progressSlider->setMaximum(duration / 1000);
}