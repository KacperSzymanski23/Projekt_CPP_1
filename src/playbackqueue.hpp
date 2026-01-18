#ifndef PLAYBACKQUEUE_HPP
#define PLAYBACKQUEUE_HPP

// Qt
#include <QObject>
#include <QString>

class PlaybackQueue : public QObject {
		Q_OBJECT

	  public:
		enum PlaybackMode : uint8_t { CurrentItemOnce = 0, CurrentItemInLoop = 1, Sequential = 2, Loop = 3, Shuffled = 4 }; // Tryby odtwarzania piosenek

		explicit PlaybackQueue(QObject *parent = nullptr, const QList<QString> &queue = {}, PlaybackMode playbackMode = Sequential);

		[[nodiscard]] PlaybackMode playbackMode() const; // Pobiera tryb odtwarzania
		void setPlaybackMode(PlaybackMode mode);         // Ustawia tryb odtwarzania

		qsizetype nextIndex(int32_t index);     // Zwraca indeks następnej piosenki
		qsizetype previousIndex(int32_t index); // Zwraca indeks poprzedniej piosenki

		[[nodiscard]] qsizetype currentIndex() const; // Pobiera aktualny indeks
		QString currentMedia();                       // Pobiera ścieżkę do pliku obecnie odtwarzanej piosenki

		[[nodiscard]] qsizetype mediaCount() const; // Zwraca liczbę piosenek w kolejce
		[[nodiscard]] bool isEmpty() const;         // Sprawdza czy kolejka jest pusta

		void addMedia(const QString &content);                     // Dodaje piosenke do kolejki
		void insertMedia(qsizetype index, const QString &content); // Dodaje piosenke do kolejki w określonym miejscu
		void removeMedia(qsizetype index);                         // Usuwa piosenke z kolejki

		void setQueue(const QList<QString> &queue); // Ustawia kolejke

		void clear(); // Usuwa wszystkie piosenki z kolejki

	  private:
		PlaybackMode m_playbackMode{PlaybackQueue::Sequential};
		qsizetype m_currentIndex{-1};
		int32_t m_orderOfPlaybackIndex{0};

		QList<int32_t> m_orderOfPlayback; // Porządek odtwarzania używany w trybie Shuffled
		QList<QString> m_queue;           // Kolejka odtwarzania

	  public slots:
		void shuffle(); // Generuje porządek odtwarzania i ustawia tryb odtwarzania na Shuffled

		void next();     // Ustawia m_currentIndex na kolejny indeks i emituje sygnał currentIndexChanged
		void previous(); // Ustawia m_currentIndex na poprzedni indeks i emituje sygnał currentIndexChanged

		void setCurrentIndex(qsizetype index); // Ustawia aktualny indeks

	  signals:
		// Sygnały emitowane przez PlaybackQueue gdy zmienia się aktualny indeks lub aktualna piosenka
		void currentIndexChanged(qsizetype index);
		void currentMediaChanged(const QString &media);

		void mediaChanged(int32_t start, int32_t end);
};

#endif // PLAYBACKQUEUE_HPP
