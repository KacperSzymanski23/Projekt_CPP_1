#ifndef LIBRARY_HPP
#define LIBRARY_HPP

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
		};

		class Album {
			  public:
				Album(const QString &title, const QUrl &coverArtPath, const QList<TrackMetadata> &tracks = {}, const QList<QUrl> &tracksPaths = {});
				Album() = default;

				void setTitle(const QString &title);
				QString getTitle() const;

				void setCoverArtPath(const QUrl &path);
				QUrl getCoverArtPath() const;

				void setData(const QList<TrackMetadata> &tracks, const QList<QUrl> &tracksPaths);
				void appendData(const TrackMetadata &track, const QUrl &path);

				TrackMetadata getTrackByIndex(uint32_t index) const;
				QList<TrackMetadata> getTracksList() const;

				QUrl getUrlByIndex(uint32_t index) const;
				QList<QUrl> getTracksPathsList() const;

			  private:
				void findCoverArt(const QUrl &path);

				QString m_title;
				QUrl m_coverArtPath;
				QList<TrackMetadata> m_tracks;
				QList<QUrl> m_tracksPaths;
		};

		class Artist {
			  public:
				explicit Artist(const QString &name, const QList<Album> &albums = {});

				void setName(const QString &name);
				QString getName() const;

				void setAlbumsList(const QList<Album> &albums);
				QList<Album> getAlbumsList() const;

				Album getAlbumByIndex(uint32_t index) const;
				Album findAlbumByTitle(const QString &albumTitle) const;

				void appendAlbum(const Album &album);

			  private:
				QString m_name;
				QList<Album> m_albums;
		};

		Library(const QUrl &libraryPath = {});

		void setLibraryPath(const QUrl &path);

		static TrackMetadata extractMetadata(const QString &path, const TagLib::FileRef &ref);

		void scanLibraryPath();

		QList<Artist> getArtistList() const;
		QList<Album> getAlbumsList() const;
		Album getAlbumByIndex(uint32_t index) const;

		Artist findArtistByName(const QString &artistName) const;
		Artist getArtistByIndex(int32_t index) const;

		void groupTracks(const QList<TrackMetadata> &tracksMetadatas, const QList<QUrl> &path);

	  private:
		QUrl m_libraryPath;
		QList<Artist> m_artists;
		QList<Album> m_albums;
};

#endif // LIBRARY_HPP
