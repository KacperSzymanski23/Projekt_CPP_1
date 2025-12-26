#include "playbackqueue.hpp"
// Qt
#include <QDebug>
#include <QRandomGenerator>
// Tracy
#include <tracy/Tracy.hpp>

PlaybackQueue::PlaybackQueue(QObject *parent, const QList<QUrl> &queue, PlaybackMode playbackMode)
	: QObject(parent)
	, m_playbackMode(playbackMode)
	, m_queue(std::move(queue)) {
}

PlaybackQueue::PlaybackMode PlaybackQueue::playbackMode() const {
		return m_playbackMode;
}

void PlaybackQueue::setPlaybackMode(PlaybackMode mode) {

		if (m_playbackMode != mode) {
				m_playbackMode = mode;

				return;
		}

		m_playbackMode = PlaybackMode::Sequential;
}

int32_t PlaybackQueue::nextIndex(int32_t index) {
		ZoneScoped;

		if (m_queue.count() == 0) {
				return -1;
		}

		int32_t nextIndex = m_currentIndex + index;

		switch (m_playbackMode) {
				case PlaybackMode::CurrentItemOnce:
						if (index != 0) {
								return -1;
						}
						return m_currentIndex;

				case PlaybackMode::CurrentItemInLoop:
						return m_currentIndex;

				case PlaybackMode::Sequential:
						if (m_queue.size() <= nextIndex) {
								nextIndex = 0;
						}

						break;

				case PlaybackMode::Loop:
						nextIndex %= m_queue.count();
						break;

				case PlaybackMode::Shuffled:
						nextIndex = m_orderOfPlayback.at(m_orderOfPlaybackIndex);

						if (m_orderOfPlaybackIndex < m_queue.size() - 1) {
								m_orderOfPlaybackIndex++;
						} else {
								m_orderOfPlaybackIndex = 0;
								m_playbackMode = PlaybackMode::Sequential;
						}

						break;
		}

		return nextIndex;
}

int32_t PlaybackQueue::previousIndex(int32_t index) {
		ZoneScoped;

		if (m_queue.count() == 0) {
				return -1;
		}

		int32_t previousIndex = m_currentIndex;

		if (previousIndex < 0) {
				previousIndex = m_queue.size();
		}

		previousIndex = previousIndex - index;

		switch (m_playbackMode) {
				case PlaybackMode::CurrentItemOnce:
						if (index != 0) {
								return -1;
						}
						return m_currentIndex;

				case PlaybackMode::CurrentItemInLoop:
						return m_currentIndex;

				case PlaybackMode::Sequential:
						if (previousIndex < 0) {
								previousIndex = m_queue.size() - 1;
						}
						break;

				case PlaybackMode::Loop:
						previousIndex %= m_queue.size();

						if (previousIndex < 0) {
								previousIndex += m_queue.size();
						}
						break;

				case PlaybackMode::Shuffled:
						previousIndex = m_orderOfPlayback.at(m_orderOfPlaybackIndex);

						if (m_orderOfPlaybackIndex > 0) {
								m_orderOfPlaybackIndex--;
						} else {
								m_orderOfPlaybackIndex = 0;
								m_playbackMode = PlaybackMode::Sequential;
						}

						break;
		}

		return previousIndex;
}

int32_t PlaybackQueue::currentIndex() const {
		return m_currentIndex;
}

QUrl PlaybackQueue::currentMedia() {
		m_currentIndex = qBound(0, m_currentIndex, m_queue.size());

		return m_queue.at(m_currentIndex);
}

int32_t PlaybackQueue::mediaCount() const {
		return m_queue.count();
}

bool PlaybackQueue::isEmpty() const {
		return m_queue.isEmpty();
}

void PlaybackQueue::addMedia(const QUrl &content) {
		m_queue.append(content);
}

void PlaybackQueue::insertMedia(int32_t index, const QUrl &content) {
		ZoneScoped;

		index = qBound(0, index, m_queue.size());

		m_queue.insert(index, content);
}

void PlaybackQueue::removeMedia(int32_t index) {
		ZoneScoped;

		index = qBound(0, index, m_queue.size() - 1);

		m_queue.remove(index);
}

void PlaybackQueue::setQueue(const QList<QUrl> &queue) {
		m_queue = std::move(queue);
}

void PlaybackQueue::clear() {
		m_queue.clear();
}

void PlaybackQueue::shuffle() {
		ZoneScoped;

		setPlaybackMode(PlaybackMode::Shuffled);

		m_orderOfPlayback.reserve(m_queue.size());

		for (int32_t i{}; i < m_queue.size(); ++i) {
				m_orderOfPlayback.append(i);
		}

		std::mt19937 rng(QRandomGenerator::global()->generate());
		std::ranges::shuffle(m_orderOfPlayback, rng);
}

void PlaybackQueue::next() {
		m_currentIndex = nextIndex(1);

		emit currentIndexChanged(m_currentIndex);
		emit currentMediaChanged(currentMedia());
}

void PlaybackQueue::previous() {
		m_currentIndex = previousIndex(1);

		emit currentIndexChanged(m_currentIndex);
		emit currentMediaChanged(currentMedia());
}

void PlaybackQueue::setCurrentIndex(int32_t index) {
		if (index < 0 || index >= m_queue.size()) {
				index = -1;
		}

		m_currentIndex = index;

		emit currentIndexChanged(m_currentIndex);
		emit currentMediaChanged(currentMedia());
}