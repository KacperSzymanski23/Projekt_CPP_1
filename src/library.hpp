#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include "collection.hpp"
// Qt
#include <QList>
#include <QString>
#include <QUrl>
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
				Album(const QString &title, const QUrl &coverArtPath, const QList<TrackMetadata> &tracks = {}, const QList<QUrl> &tracksPaths = {});
				Album() = default;

				void setCoverArtPath(const QUrl &path);
				[[nodiscard]] QUrl getCoverArtPath() const;

				void setData(const QList<TrackMetadata> &tracks, const QList<QUrl> &paths);
				void appendData(const TrackMetadata &track, const QUrl &path);

				[[nodiscard]] QUrl getUrl(qsizetype index) const;
				[[nodiscard]] QList<QUrl> getTracksPathsList() const;

			  private:
				void findCoverArt(const QUrl &path);

				QUrl m_coverArtPath;
				QList<QUrl> m_tracksPaths;
		};

		class Artist : public Collection<Album> {
			  public:
				explicit Artist(const QString &name, const QList<Album> &albums = {});

				[[nodiscard]] Album findAlbum(const QString &title) const;

				void appendAlbum(const Album &album);
		};

		explicit Library(const QUrl &libraryPath = {});

		void setLibraryPath(const QUrl &path);

		static TrackMetadata extractMetadata(const QString &path, const TagLib::FileRef &ref);

		void scanLibraryPath();

		[[nodiscard]] QList<Artist> getArtistList() const;
		[[nodiscard]] QList<Album> getAlbumsList() const;
		[[nodiscard]] Album getAlbum(qsizetype index) const;

		[[nodiscard]] Artist getArtist(const QString &name) const;
		[[nodiscard]] Artist getArtist(qsizetype index) const;

		void groupTracks(const QHash<QString, QHash<QString, QList<QPair<TrackMetadata, QUrl>>>> &collector);

	  private:
		QUrl m_libraryPath;
		QList<Artist> m_artists;
		// TODO(kacper): Sprawdzić czy można zastąpić m_artists
		// Ta lista zawiera dane znajdujace się już w m_artists
		// więc nieporzebnie zaśmieca pamięć
		QList<Album> m_albums;
};

#endif // LIBRARY_HPP
