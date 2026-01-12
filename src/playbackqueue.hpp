#ifndef PLAYBACKQUEUE_HPP
#define PLAYBACKQUEUE_HPP

// Qt
#include <QObject>
#include <QUrl>

class PlaybackQueue : public QObject {
		Q_OBJECT

	  public:
		enum PlaybackMode : uint8_t { CurrentItemOnce = 0, CurrentItemInLoop = 1, Sequential = 2, Loop = 3, Shuffled = 4 };

		explicit PlaybackQueue(QObject *parent = nullptr, const QList<QUrl> &queue = {}, PlaybackMode playbackMode = Sequential);

		[[nodiscard]] PlaybackMode playbackMode() const;
		void setPlaybackMode(PlaybackMode mode);

		qsizetype nextIndex(int32_t index);
		qsizetype previousIndex(int32_t index);

		[[nodiscard]] qsizetype currentIndex() const;
		QUrl currentMedia();

		[[nodiscard]] qsizetype mediaCount() const;
		bool isEmpty() const;

		void addMedia(const QUrl &content);
		void insertMedia(qsizetype index, const QUrl &content);
		void removeMedia(qsizetype index);

		void setQueue(const QList<QUrl> &queue);

		void clear();

	  private:
		PlaybackMode m_playbackMode{PlaybackQueue::Sequential};
		qsizetype m_currentIndex{-1};
		int32_t m_orderOfPlaybackIndex{0};

		QList<int32_t> m_orderOfPlayback;
		QList<QUrl> m_queue;

	  public slots:
		void shuffle();

		void next();
		void previous();

		void setCurrentIndex(qsizetype index);

	  signals:
		void currentIndexChanged(qsizetype index);
		void currentMediaChanged(const QUrl &media);

		void mediaChanged(int32_t start, int32_t end);
};

#endif // PLAYBACKQUEUE_HPP
