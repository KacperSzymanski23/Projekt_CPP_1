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

				void setCoverArtPath(const QString &path);
				[[nodiscard]] QString getCoverArtPath() const;

				void setData(const QList<TrackMetadata> &tracks, const QList<QString> &paths);
				void appendData(const TrackMetadata &track, const QString &path);

				[[nodiscard]] QString getPath(qsizetype index) const;
				[[nodiscard]] QList<QString> getTracksPathsList() const;

			  private:
				void findCoverArt(const QString &path);

				QString m_coverArtPath;
				QList<QString> m_tracksPaths;
		};

		class Artist : public Collection<Album> {
			  public:
				explicit Artist(const QString &name, const QList<Album> &albums = {});

				[[nodiscard]] Album findAlbum(const QString &title) const;

				void appendAlbum(const Album &album);
		};

		explicit Library(const QString &libraryPath = {});

		void setLibraryPath(const QString &path);

		static QPair<TrackMetadata, QString> extractMetadata(const QString &path);

		void scanLibraryPath();

		[[nodiscard]] QList<Artist> getArtistList() const;
		[[nodiscard]] QList<Album> getAlbumsList() const;
		[[nodiscard]] Album getAlbum(qsizetype index) const;

		[[nodiscard]] Artist getArtist(const QString &name) const;
		[[nodiscard]] Artist getArtist(qsizetype index) const;

		void groupTracks(QList<std::pair<TrackMetadata, QString>> &data);

	  private:
		QString m_libraryPath;
		QList<Artist> m_artists;
		// TODO(kacper): Sprawdzić czy można zastąpić m_artists
		// Ta lista zawiera dane znajdujace się już w m_artists
		// więc nieporzebnie zaśmieca pamięć
		QList<Album> m_albums;
};

#endif // LIBRARY_HPP
