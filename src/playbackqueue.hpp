#ifndef PLAYBACKQUEUE_HPP
#define PLAYBACKQUEUE_HPP

// Qt
#include <QObject>
#include <QUrl>
// STD
#include <vector>

class PlaybackQueue : public QObject {
		Q_OBJECT

	  public:
		enum PlaybackMode { CurrentItemOnce = 0, CurrentItemInLoop = 1, Sequential = 2, Loop = 3, Shuffled = 4 };

		explicit PlaybackQueue(QObject *parent = nullptr, const QList<QUrl> &queue = {}, PlaybackMode playbackMode = Sequential);

		PlaybackMode playbackMode() const;
		void setPlaybackMode(PlaybackMode mode);

		int32_t nextIndex(int32_t index);
		int32_t previousIndex(int32_t index);

		int32_t currentIndex() const;
		QUrl currentMedia();

		int32_t mediaCount() const;
		bool isEmpty() const;

		void addMedia(const QUrl &content);
		void insertMedia(int32_t index, const QUrl &content);
		void removeMedia(int32_t pos);

		void setQueue(const QList<QUrl> &queue);

		void clear();

	  private:
		PlaybackMode m_playbackMode{PlaybackQueue::Sequential};
		int32_t m_currentIndex{-1};
		int32_t m_orderOfPlaybackIndex{0};

		QList<int32_t> m_orderOfPlayback{};
		QList<QUrl> m_queue;

	  public slots:
		void shuffle();

		void next();
		void previous();

		void setCurrentIndex(int32_t index);

	  signals:
		void currentIndexChanged(int32_t index);
		void currentMediaChanged(const QUrl &media);

		void mediaChanged(int32_t start, int32_t end);
};

#endif // PLAYBACKQUEUE_HPP
