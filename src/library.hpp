#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include "collection.hpp"
// Qt
#include <QList>
#include <QString>
// TagLib
#include <taglib/fileref.h>

class Library {
	  public:
		struct TrackMetadata {
				uint32_t number;
				QString title;
				QString album;
				QString artist;
				QString duration;
				uint32_t year;
				QString bitrate;
				QString fileSize;
				QString path;
		};

		class Album : public Collection<TrackMetadata> {
			  public:
				Album(const QString &title, const QString &coverArtPath, const QList<TrackMetadata> &tracks = {}, const QList<QString> &tracksPaths = {});
				Album() = default;

				void setCoverArtPath(const QString &path);     // Ustawia ścieżke do okładki albumu
				[[nodiscard]] QString getCoverArtPath() const; // Pobiera ścieżke do okładki albumu

				void setData(const QList<TrackMetadata> &tracks, const QList<QString> &paths); // Ustawia ścieżki do plików i metadane utworów
				void appendData(const TrackMetadata &track, const QString &path);              // Dodaje ścieżki do plików i metadane utworów

				[[nodiscard]] QString getPath(qsizetype index) const;    // Pobiera ścieżke do pliku o podanym indeksie
				[[nodiscard]] QList<QString> getTracksPathsList() const; // Pobiera ścieżki do plików utworów

			  private:
				void findCoverArt(const QString &path); // Szuka okładki albumu

				QString m_coverArtPath;       // Ścieżka do okładki albumu
				QList<QString> m_tracksPaths; // Ścieżki do plikow audio
		};

		class Artist : public Collection<Album> {
			  public:
				explicit Artist(const QString &name, const QList<Album> &albums = {});

				[[nodiscard]] Album findAlbum(const QString &title) const; // Zwraca album o podanej nazwie

				void appendAlbum(const Album &album); // Dodaje album do listy
		};

		explicit Library(const QString &libraryPath = {});

		void setLibraryPath(const QString &path); // Ustawia ścieżke do biblioteki

		static QPair<TrackMetadata, QString> extractMetadata(const QString &path); // Pobiera metadane z pliku audio i zwraca
		                                                                           // je w postaci pary (TrackMetadata, QString)

		void scanLibraryPath(); // Skanuje bibliotekę

		[[nodiscard]] QList<Artist> getArtistList() const;   // Pobiera listę artystów
		[[nodiscard]] QList<Album> getAlbumsList() const;    // Pobiera listę albumów
		[[nodiscard]] Album getAlbum(qsizetype index) const; // Pobiera album o podanym indeksie

		[[nodiscard]] Artist getArtist(const QString &name) const; // Pobiera artystę o podanej nazwie
		[[nodiscard]] Artist getArtist(qsizetype index) const;     // Pobiera artystę o podanym indeksie

		void groupTracks(QList<std::pair<TrackMetadata, QString>> &data); // Grupuje utwory w albumy a po albumach w artystów

	  private:
		QString m_libraryPath;   // Ścieżka do biblioteki
		QList<Artist> m_artists; // Lista artystów

		// TODO(kacper): Sprawdzić czy można zastąpić m_artists
		// Ta lista zawiera dane znajdujace się już w m_artists
		// więc nieporzebnie zaśmieca pamięć
		QList<Album> m_albums; // Lista albumów
};

#endif // LIBRARY_HPP
