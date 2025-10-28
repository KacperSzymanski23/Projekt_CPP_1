#include "playercontrols.hpp"
// Qt
#include <QTime>
#include <unistd.h>

PlayerControls::PlayerControls(QWidget *parent)
	: QWidget(parent)
	, m_progressSlider(new QSlider())
	, m_volumeSlider(new QSlider())
	, m_playbackControllLayout(new QHBoxLayout(this)) {

		m_playbackControllLayout->setSpacing(0);

		const QSize ICON_SIZE = QSize(24, 24);

		const QIcon LOOP_ICON = QIcon::fromTheme("media-playlist-repeat-rtl-symbolic");
		m_loopButton = new QToolButton(this);
		m_loopButton->setIcon(LOOP_ICON);
		m_loopButton->setIconSize(ICON_SIZE);
		m_loopButton->setToolTip(tr("Loop Playback"));
		m_loopButton->setCheckable(true);
		m_playbackControllLayout->addWidget(m_loopButton);

		const QIcon SHUFFLE_ICON = QIcon::fromTheme("media-playlist-shuffle-symbolic");
		m_shuffleButton = new QToolButton(this);
		m_shuffleButton->setIcon(SHUFFLE_ICON);
		m_shuffleButton->setIconSize(ICON_SIZE);
		m_shuffleButton->setToolTip(tr("Shuffle"));
		m_shuffleButton->setCheckable(true);
		m_playbackControllLayout->addWidget(m_shuffleButton);

		const QIcon PREVIOUS_SONG_ICON = QIcon::fromTheme("media-skip-backward-symbolic");
		m_previousButton = new QToolButton(this);
		m_previousButton->setIcon(PREVIOUS_SONG_ICON);
		m_previousButton->setIconSize(ICON_SIZE);
		m_previousButton->setToolTip(tr("Previous Song"));
		m_playbackControllLayout->addWidget(m_previousButton);
		connect(m_previousButton, &QToolButton::clicked, this, &PlayerControls::previous);

		const QIcon START_ICON = QIcon::fromTheme("media-playback-start-symbolic");
		const QIcon PAUSE_ICON = QIcon::fromTheme("media-playback-pause-symbolic");
		m_playPauseButton = new QToolButton(this);
		m_playPauseButton->setIcon(START_ICON);
		m_playPauseButton->setIconSize(ICON_SIZE);
		m_playPauseButton->setToolTip(tr("Start"));
		m_playbackControllLayout->addWidget(m_playPauseButton);
		connect(m_playPauseButton, &QToolButton::clicked, this, &PlayerControls::playPauseClicked);

		const QIcon NEXT_SONG_ICON = QIcon::fromTheme("media-skip-forward-symbolic");
		m_nextButton = new QToolButton(this);
		m_nextButton->setIcon(NEXT_SONG_ICON);
		m_nextButton->setIconSize(ICON_SIZE);
		m_nextButton->setToolTip(tr("Next Song"));
		m_playbackControllLayout->addWidget(m_nextButton);
		connect(m_nextButton, &QToolButton::clicked, this, &PlayerControls::next);

		m_progressSlider->setOrientation(Qt::Horizontal);
		m_playbackControllLayout->addWidget(m_progressSlider);
		connect(m_progressSlider, &QSlider::sliderMoved, this, &PlayerControls::progressSliderMoved);

		m_progressLabel = new QLabel(tr(" 00:00/00:00 "));
		m_playbackControllLayout->addWidget(m_progressLabel);

		const QIcon MUTED_ICON = QIcon::fromTheme("audio-volume-muted-symbolic");
		const QIcon HIGH_VOLUME_ICON = QIcon::fromTheme("audio-volume-high-symbolic");
		const QIcon MED_VOLUME_ICON = QIcon::fromTheme("audio-volume-medium-symbolic");
		const QIcon LOW_VOLUME_ICON = QIcon::fromTheme("audio-volume-low-symbolic");
		m_audioButton = new QToolButton(this);
		m_audioButton->setIcon(HIGH_VOLUME_ICON);
		m_audioButton->setIconSize(ICON_SIZE);
		m_audioButton->setToolTip(tr("Volume"));
		m_playbackControllLayout->addWidget(m_audioButton);
		connect(m_audioButton, &QToolButton::clicked, this, &PlayerControls::muteClicked);

		m_volumeSlider->setOrientation(Qt::Horizontal);
		m_volumeSlider->setMaximumWidth(175);
		m_volumeSlider->setValue(100);
		m_playbackControllLayout->addWidget(m_volumeSlider);

		connect(m_volumeSlider, &QSlider::valueChanged, this, &PlayerControls::volumeSliderValueChanged);

		const QIcon FAVORITE_ICON = QIcon::fromTheme("emblem-favorite-symbolic");
		m_favoriteButton = new QToolButton(this);
		m_favoriteButton->setIcon(FAVORITE_ICON);
		m_favoriteButton->setIconSize(ICON_SIZE);
		m_favoriteButton->setToolTip(tr("Add Favorite"));
		m_favoriteButton->setCheckable(true);
		m_playbackControllLayout->addWidget(m_favoriteButton);

		this->setLayout(m_playbackControllLayout);
}

PlayerControls::~PlayerControls() = default;

float PlayerControls::getVolume() const {
		return static_cast<float>(m_volumeSlider->value()) / 100.0F;
}

bool PlayerControls::isMuted() const {
		return m_playerMuted;
}

void PlayerControls::updateProgressLabel(int32_t progress) {
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
		emit play();
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
}

void PlayerControls::progressSliderMoved() {
		qDebug() << m_progressSlider->value();
		emit changeProgress(m_progressSlider->value() * 1000);
}

void PlayerControls::setVolume(float volume) {
		const int VOLUME = static_cast<int>(volume * 100);

		m_volumeSlider->setValue(VOLUME);
}

void PlayerControls::setMuted(bool muted) {
		if (muted != m_playerMuted) {
				m_playerMuted = muted;

				m_audioButton->setIcon(
					muted ? QIcon::fromTheme("audio-volume-muted-symbolic") : QIcon::fromTheme("audio-volume-high-symbolic")
				);
		}
}

void PlayerControls::setShuffle(bool shuffle) {
		if (shuffle != m_playerShuffled) {
				m_playerShuffled = shuffle;

				m_shuffleButton->setIcon(
					shuffle ? QIcon::fromTheme("media-playlist-shuffle-symbolic") : QIcon::fromTheme("media-playlist-repeat-symbolic")
				);
		}
}

void PlayerControls::setLoop(bool loop) {
		if (loop != m_playerLooped) {
				m_playerLooped = loop;

				m_loopButton->setIcon(
					loop ? QIcon::fromTheme("media-playlist-repeat-symbolic") : QIcon::fromTheme("media-playlist-repeat-single-symbolic")
				);
		}
}

void PlayerControls::setFavorite(bool favorite) {
		if (favorite != m_playerFavorite) {
				m_playerFavorite = favorite;

				m_favoriteButton->setIcon(
					favorite ? QIcon::fromTheme("emblem-favorite-symbolic") : QIcon::fromTheme("emblem-favorite-symbolic-symbolic")
				);
		}
}

void PlayerControls::setTrackProgress(int32_t progress) {
		m_progressSlider->setValue(progress / 1000);

		updateProgressLabel(progress / 1000);
}

void PlayerControls::setTrackDuration(const int32_t DURATION) {
		m_duration = DURATION / 1000;
		m_progressSlider->setMaximum(DURATION / 1000);
}