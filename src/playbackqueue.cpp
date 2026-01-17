#include "playbackqueue.hpp"
#include "logs.hpp"
// Qt
#include <QRandomGenerator>
// Tracy
#include <tracy/Tracy.hpp>

PlaybackQueue::PlaybackQueue(QObject *parent, const QList<QString> &queue, PlaybackMode playbackMode)
	: QObject(parent)
	, m_playbackMode(playbackMode)
	, m_queue(queue) {
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

qsizetype PlaybackQueue::nextIndex(int32_t index) {
		ZoneScoped;

		if (m_queue.isEmpty()) {
				logCreate("Queue is empty");
				return -1;
		}

		qsizetype nextIndex = m_currentIndex + index;

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

qsizetype PlaybackQueue::previousIndex(int32_t index) {
		ZoneScoped;

		if (m_queue.isEmpty()) {
				logCreate("Queue is empty");
				return -1;
		}

		qsizetype previousIndex = m_currentIndex;

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

qsizetype PlaybackQueue::currentIndex() const {
		return m_currentIndex;
}

QString PlaybackQueue::currentMedia() {

		if (m_currentIndex == -1) {
				logCreate("Invalid index");
				return {};
		}

		m_currentIndex = qBound(0, m_currentIndex, m_queue.size());

		return m_queue.at(m_currentIndex);
}

qsizetype PlaybackQueue::mediaCount() const {
		return m_queue.count();
}

bool PlaybackQueue::isEmpty() const {
		return m_queue.isEmpty();
}

void PlaybackQueue::addMedia(const QString &content) {
		m_queue.append(content);
}

void PlaybackQueue::insertMedia(qsizetype index, const QString &content) {
		ZoneScoped;

		index = qBound(0, index, m_queue.size());

		m_queue.insert(index, content);
}

void PlaybackQueue::removeMedia(qsizetype index) {
		ZoneScoped;

		index = qBound(0, index, m_queue.size() - 1);

		m_queue.remove(index);
}

void PlaybackQueue::setQueue(const QList<QString> &queue) {
		m_queue = queue;
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
		ZoneScoped;

		m_currentIndex = nextIndex(1);

		emit currentIndexChanged(m_currentIndex);
		emit currentMediaChanged(currentMedia());
}

void PlaybackQueue::previous() {
		ZoneScoped;

		m_currentIndex = previousIndex(1);

		emit currentIndexChanged(m_currentIndex);
		emit currentMediaChanged(currentMedia());
}

void PlaybackQueue::setCurrentIndex(qsizetype index) {
		ZoneScoped;

		if (index < 0 || index >= m_queue.size()) {
				logCreate("Index out of range: " + std::to_string(index));
				index = -1;
		}

		m_currentIndex = index;

		emit currentIndexChanged(m_currentIndex);
		emit currentMediaChanged(currentMedia());
}