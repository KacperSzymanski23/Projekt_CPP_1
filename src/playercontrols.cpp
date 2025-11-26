#include "playercontrols.hpp"
// Qt
#include <QTime>
// STD
#include <unistd.h>

PlayerControls::PlayerControls(QWidget *parent)
	: QWidget(parent)
	, m_progressSlider(new QSlider())
	, m_volumeSlider(new QSlider())
	, m_loopButton(new QToolButton(this))
	, m_shuffleButton(new QToolButton(this))
	, m_previousButton(new QToolButton(this))
	, m_playPauseButton(new QToolButton(this))
	, m_nextButton(new QToolButton(this))
	, m_audioButton(new QToolButton(this))
	, m_favoriteButton(new QToolButton(this))
	, m_playbackControlsLayout(new QHBoxLayout(this)) {

		m_playbackControlsLayout->setSpacing(0);

		constexpr QSize ICON_SIZE = QSize(24, 24);

		const QIcon LOOP_ICON = QIcon::fromTheme(":/qlementine/icons/16/media/loop.svg");

		m_loopButton->setIcon(LOOP_ICON);
		m_loopButton->setIconSize(ICON_SIZE);
		m_loopButton->setToolTip(tr("Loop Playback"));
		m_loopButton->setCheckable(true);
		m_playbackControlsLayout->addWidget(m_loopButton);

		const QIcon SHUFFLE_ICON = QIcon::fromTheme(":/qlementine/icons/16/media/shuffle.svg");

		m_shuffleButton->setIcon(SHUFFLE_ICON);
		m_shuffleButton->setIconSize(ICON_SIZE);
		m_shuffleButton->setToolTip(tr("Shuffle"));
		m_shuffleButton->setCheckable(true);
		m_playbackControlsLayout->addWidget(m_shuffleButton);

		const QIcon PREVIOUS_SONG_ICON = QIcon::fromTheme(":/qlementine/icons/16/media/seek-backward.svg");

		m_previousButton->setIcon(PREVIOUS_SONG_ICON);
		m_previousButton->setIconSize(ICON_SIZE);
		m_previousButton->setToolTip(tr("Previous Song"));
		m_playbackControlsLayout->addWidget(m_previousButton);
		connect(m_previousButton, &QToolButton::clicked, this, &PlayerControls::previous);

		const QIcon START_ICON = QIcon::fromTheme(":/qlementine/icons/16/media/play.svg");

		m_playPauseButton->setIcon(START_ICON);
		m_playPauseButton->setIconSize(ICON_SIZE);
		m_playPauseButton->setToolTip(tr("Start"));
		m_playbackControlsLayout->addWidget(m_playPauseButton);
		connect(m_playPauseButton, &QToolButton::clicked, this, &PlayerControls::playPauseClicked);

		const QIcon NEXT_SONG_ICON = QIcon::fromTheme(":/qlementine/icons/16/media/seek-forward.svg");

		m_nextButton->setIcon(NEXT_SONG_ICON);
		m_nextButton->setIconSize(ICON_SIZE);
		m_nextButton->setToolTip(tr("Next Song"));
		m_playbackControlsLayout->addWidget(m_nextButton);
		connect(m_nextButton, &QToolButton::clicked, this, &PlayerControls::next);

		m_progressSlider->setOrientation(Qt::Horizontal);
		m_playbackControlsLayout->addWidget(m_progressSlider);
		connect(m_progressSlider, &QSlider::sliderMoved, this, &PlayerControls::progressSliderMoved);

		m_progressLabel = new QLabel(tr(" 00:00/00:00 "));
		m_playbackControlsLayout->addWidget(m_progressLabel);

		const QIcon HIGH_VOLUME_ICON = QIcon::fromTheme(":/qlementine/icons/16/audio/speaker-2.svg");

		m_audioButton->setIcon(HIGH_VOLUME_ICON);
		m_audioButton->setIconSize(ICON_SIZE);
		m_audioButton->setToolTip(tr("Volume"));
		m_playbackControlsLayout->addWidget(m_audioButton);
		connect(m_audioButton, &QToolButton::clicked, this, &PlayerControls::muteClicked);

		m_volumeSlider->setOrientation(Qt::Horizontal);
		m_volumeSlider->setMaximumWidth(175);
		m_volumeSlider->setValue(100);
		m_playbackControlsLayout->addWidget(m_volumeSlider);

		connect(m_volumeSlider, &QSlider::valueChanged, this, &PlayerControls::volumeSliderValueChanged);

		const QIcon FAVORITE_ICON = QIcon::fromTheme(":/qlementine/icons/16/shape/heart.svg");

		m_favoriteButton->setIcon(FAVORITE_ICON);
		m_favoriteButton->setIconSize(ICON_SIZE);
		m_favoriteButton->setToolTip(tr("Add Favorite"));
		m_favoriteButton->setCheckable(true);
		m_playbackControlsLayout->addWidget(m_favoriteButton);

		connect(m_favoriteButton, &QToolButton::clicked, this, &PlayerControls::favoriteClicked);

		this->setLayout(m_playbackControlsLayout);
}

PlayerControls::~PlayerControls() = default;

float PlayerControls::getVolume() const {
		return static_cast<float>(m_volumeSlider->value()) / 100.0F;
}

bool PlayerControls::isMuted() const {
		return m_playerMuted;
}

void PlayerControls::updateProgressLabel(int32_t progress) const {
		const QTime PROGRESS_TIME((progress / 3600) % 60, (progress / 60) % 60, progress % 60);
		const QTime DURATION_TIME((m_duration / 3600) % 60, (m_duration / 60) % 60, m_duration % 60);

		QString timeFormat{"mm:ss"};
		if (m_duration > 3600) {
				timeFormat = "hh:mm:ss";
		}

		const QString PROGRESS_LABEL_TEXT = PROGRESS_TIME.toString(timeFormat) + "/" + DURATION_TIME.toString(timeFormat);

		m_progressLabel->setText(PROGRESS_LABEL_TEXT);
}

void PlayerControls::playPauseClicked() {
	m_playerState = !m_playerState;

	const QIcon START_ICON = QIcon::fromTheme(":/qlementine/icons/16/media/play.svg");
	const QIcon PAUSE_ICON = QIcon::fromTheme(":/qlementine/icons/16/media/pause.svg");

	if (m_playerState) {
		emit play();
		m_playPauseButton->setIcon(PAUSE_ICON);
	}else {
		emit pause();
		m_playPauseButton->setIcon(START_ICON);
	}
}

void PlayerControls::muteClicked() {
		emit changeMuteState(!m_playerMuted);
}

void PlayerControls::volumeSliderValueChanged() {
		const float VOLUME = static_cast<float>(m_volumeSlider->value());

		emit changeVolume(VOLUME / 100.0F);
}

void PlayerControls::shuffleClicked() {
		emit changeShuffleState(!m_playerShuffled);
}

void PlayerControls::loopClicked() {
		emit changeLoopedState(!m_playerLooped);
}

void PlayerControls::favoriteClicked() {
		emit changeFavoriteState(!m_playerFavorite);
		setFavorite(!m_playerFavorite);
}

void PlayerControls::progressSliderMoved() {
		emit changeProgress(m_progressSlider->value() * 1000);
}

void PlayerControls::setVolume(float volume) const {
		const int VOLUME = static_cast<int>(volume * 100);

		m_volumeSlider->setValue(VOLUME);
}

void PlayerControls::setMuted(bool muted) {
		if (muted != m_playerMuted) {
				m_playerMuted = muted;

				const QIcon HIGH_VOLUME_ICON = QIcon::fromTheme(":/qlementine/icons/16/audio/speaker-2.svg");
				const QIcon MUTE_ICON = QIcon::fromTheme(":/qlementine/icons/16/audio/speaker-mute.svg");

				if (muted) {
						m_audioButton->setIcon(MUTE_ICON);
				} else {
						m_audioButton->setIcon(HIGH_VOLUME_ICON);
				}
		}
}

void PlayerControls::setShuffle(bool shuffle) {
		if (shuffle != m_playerShuffled) {
				m_playerShuffled = shuffle;

				const QIcon PLAYLIST_SHUFFLE_ICON = QIcon::fromTheme(":/qlementine/icons/16/media/shuffle.svg");
				m_shuffleButton->setIcon(PLAYLIST_SHUFFLE_ICON);
		}
}

void PlayerControls::setLoop(bool loop) {
		if (loop != m_playerLooped) {
				m_playerLooped = loop;

				const QIcon PLAYLIST_LOOP_ICON = QIcon::fromTheme(":/qlementine/icons/16/media/loop.svg");
				m_loopButton->setIcon(PLAYLIST_LOOP_ICON);
		}
}

void PlayerControls::setFavorite(bool favorite) {
		if (favorite != m_playerFavorite) {
				m_playerFavorite = favorite;

				const QIcon FAVORITE_ICON = QIcon::fromTheme(":/qlementine/icons/16/shape/heart-filled.svg");
				const QIcon NO_FAVORITE_ICON = QIcon::fromTheme(":/qlementine/icons/16/shape/heart.svg");
				if (favorite) {
						m_favoriteButton->setIcon(FAVORITE_ICON);
				} else {
						m_favoriteButton->setIcon(NO_FAVORITE_ICON);
				}
		}
}

void PlayerControls::setTrackProgress(int32_t progress) const {
		m_progressSlider->setValue(progress / 1000);

		updateProgressLabel(progress / 1000);
}

void PlayerControls::setTrackDuration(int32_t duration) {
		m_duration = duration / 1000;
		m_progressSlider->setMaximum(duration / 1000);
}