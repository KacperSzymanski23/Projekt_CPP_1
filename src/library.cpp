#include "library.hpp"
// Qt
#include <QDir>
#include <QDirListing>
#include <QFileInfo>
#include <QFuture>
#include <QtConcurrent>
// Tracy
#include <tracy/Tracy.hpp>

Library::Album::Album(const QString &title, const QUrl &coverArtPath, const QList<TrackMetadata> &tracks, const QList<QUrl> &tracksPaths)
	: m_title(title)
	, m_coverArtPath(coverArtPath)
	, m_tracks(tracks)
	, m_tracksPaths(tracksPaths) {
		ZoneScoped;

		findCoverArt(m_coverArtPath);
}

void Library::Album::setTitle(const QString &title) {
		m_title = title;
}

QString Library::Album::getTitle() const {
		return m_title;
}

void Library::Album::setCoverArtPath(const QUrl &path) {
		m_coverArtPath = path;
}

QUrl Library::Album::getCoverArtPath() const {
		return m_coverArtPath;
}

void Library::Album::setData(const QList<TrackMetadata> &tracks, const QList<QUrl> &tracksPaths) {
		ZoneScoped;

		m_tracks = std::move(tracks);
		m_tracksPaths = std::move(tracksPaths);

		findCoverArt(m_coverArtPath);
}

void Library::Album::appendData(const TrackMetadata &track, const QUrl &path) {
		ZoneScoped;

		m_tracks.append(track);
		m_tracksPaths.append(path);
}

Library::TrackMetadata Library::Album::getTrackByIndex(uint32_t index) const {
		return m_tracks.at(index);
}

QList<Library::TrackMetadata> Library::Album::getTracksList() const {
		ZoneScoped;

		return m_tracks;
}

QUrl Library::Album::getUrlByIndex(uint32_t index) const {
		return m_tracksPaths.at(index);
}

QList<QUrl> Library::Album::getTracksPathsList() const {
		ZoneScoped;

		return m_tracksPaths;
}

void Library::Album::findCoverArt(const QUrl &path) {
		ZoneScoped;

		const QStringList COVER_FILES = {"cover.jpg", "cover.jpeg", "cover.png", "cover.webp"}; // Wspierane typy plików dla grafiki okładki

		const QDir DIRECTORY{path.toString()};
		for (const QString &fileName : COVER_FILES) {
				const QFileInfo FILE_INFO{DIRECTORY, fileName};

				if (FILE_INFO.exists() && FILE_INFO.isFile()) {
						m_coverArtPath = FILE_INFO.absoluteFilePath();

						return;
				}
		}
}

Library::Artist::Artist(const QString &name, const QList<Album> &albums)
	: m_name(name)
	, m_albums(albums) {
}

void Library::Artist::setName(const QString &name) {
		m_name = name;
}

QString Library::Artist::getName() const {
		return m_name;
}

void Library::Artist::setAlbumsList(const QList<Album> &albums) {
		ZoneScoped;

		m_albums = std::move(albums);
}

QList<Library::Album> Library::Artist::getAlbumsList() const {
		return m_albums;
}

void Library::Artist::appendAlbum(const Album &album) {
		m_albums.append(std::move(album));
}

Library::Library(const QUrl &libraryPath)
	: m_libraryPath(libraryPath) {
}

void Library::setLibraryPath(const QUrl &path) {
		m_libraryPath = path;
}

Library::TrackMetadata Library::extractMetadata(const QString &path, const TagLib::FileRef &ref) {
		ZoneScoped;

		if (ref.isNull() || ref.tag() == nullptr) {
				return {};
		}

		if (path.isEmpty()) {
				return {};
		}

		const QFileInfo FILE_INFO{path};
		const TagLib::Tag *TAG{ref.tag()};
		const TagLib::AudioProperties *AUDIO_PROPERTIES{ref.audioProperties()};

		const uint32_t NUMBER{TAG->track()};
		const QString TITLE{QString::fromStdWString(TAG->title().toWString())};
		const QString ALBUM{QString::fromStdWString(TAG->album().toWString())};
		const QString ARTIST{QString::fromStdWString(TAG->artist().toWString())};
		const uint32_t YEAR{TAG->year()};

		const int32_t DURATION_IN_SECONDS{AUDIO_PROPERTIES->lengthInSeconds()};
		const int32_t BITRATE{AUDIO_PROPERTIES->bitrate()};

		const QTime DURATION{0, DURATION_IN_SECONDS / 60, DURATION_IN_SECONDS % 60};

		constexpr float MIB = 1024.0F * 1024.0F;
		const float FILE_SIZE = static_cast<float>(FILE_INFO.size()) / MIB;

		return {
			.number = NUMBER,
			.title = TITLE,
			.album = ALBUM,
			.artist = ARTIST,
			.duration = DURATION.toString("mm:ss"),
			.year = YEAR,
			.bitrate = QString::number(BITRATE) + " kbps",
			.fileSize = QString::number(FILE_SIZE, 'f', 1) + " MiB",
		};
}

void Library::scanLibraryPath() {
		ZoneScoped;

		if (m_libraryPath.isEmpty()) {
				m_libraryPath = QDir::homePath() + "/Music";
		}
		QDir library{m_libraryPath.toString()};

		if (!library.exists()) {
				return;
		}

		constexpr auto FLAGS = QDirListing::IteratorFlag::Recursive | QDirListing::IteratorFlag::FilesOnly;
		const QStringList AUDIO_FILE_FILTER = {"*.mp4", "*.mp3", "*.flac", "*.wav", "*.ogg", "*.opus", "*.m4a", "*.mka"}; // Wspierane typy plików

		QList<QUrl> tracksPaths;
		for (const auto &file : QDirListing(library.path(), AUDIO_FILE_FILTER, FLAGS)) {
				tracksPaths.append(file.absoluteFilePath());
		}

		if (tracksPaths.isEmpty()) {
				return;
		}

		QFuture<TrackMetadata> future = QtConcurrent::mapped(tracksPaths, [](const QUrl &filePath) -> TrackMetadata {
				TagLib::FileRef fileRef{QFile::encodeName(filePath.toString()).constData()};

				return extractMetadata(filePath.toString(), fileRef);
		});
		future.waitForFinished();

		const auto results = future.results();

		groupTracks(results, tracksPaths);
}

QList<Library::Artist> Library::getArtistList() const {
		ZoneScoped;

		return m_artists;
}

Library::Artist Library::findArtistByName(const QString &artistName) const {
		ZoneScoped;

		for (const auto &artist : m_artists) {
				if (artist.getName() == artistName) {
						return artist;
				}
		}

		return Artist(nullptr);
}

Library::Album Library::findAlbumByTitle(const QString &albumTitle) const {
		ZoneScoped;

		for (const auto &album : m_albums) {
				if (album.getTitle() == albumTitle) {
						return album;
				}
		}

		return Album();
}

QList<Library::Album> Library::getAlbumsList() const {
		ZoneScoped;

		return m_albums;
}

void Library::groupTracks(const QList<TrackMetadata> &tracks, const QList<QUrl> &paths) {
		ZoneScoped;

		QHash<QString, QHash<QString, QList<QPair<TrackMetadata, QUrl>>>> collector;

		const int32_t NUMBER_OF_TRACKS = tracks.size();
		const int32_t NUMBER_OF_PATHS = paths.size();
		const int32_t NUMBER_OF_ELEMENTS = qMax(NUMBER_OF_TRACKS, NUMBER_OF_PATHS);

		for (int32_t i{0}; i < NUMBER_OF_ELEMENTS; ++i) {
				if (i >= NUMBER_OF_TRACKS) {
						break;
				}

				const TrackMetadata &TRACK = tracks.at(i);
				QUrl path{};
				if (i < NUMBER_OF_PATHS) {
						path = paths.at(i);
				}

				collector[TRACK.artist][TRACK.album].append(qMakePair(TRACK, path));
		}

		m_artists.reserve(collector.size());

		for (auto itArtist = collector.constBegin(); itArtist != collector.constEnd(); ++itArtist) {
				Artist artist{itArtist.key()};

				const QHash<QString, QList<QPair<TrackMetadata, QUrl>>> &ALBUMS_MAP = itArtist.value();

				for (auto itAlbum = ALBUMS_MAP.constBegin(); itAlbum != ALBUMS_MAP.constEnd(); ++itAlbum) {
						QList<QPair<TrackMetadata, QUrl>> pairs = itAlbum.value();

						QFileInfo fileInfo{pairs.first().second.toString()};
						Album album{itAlbum.key(), fileInfo.absolutePath()};

						std::ranges::stable_sort(pairs, [](const QPair<TrackMetadata, QUrl> &left, const QPair<TrackMetadata, QUrl> &right) {
								return left.first.number < right.first.number;
						});

						for (const auto &[track, path] : pairs) {
								album.appendData(track, path);
						}

						artist.appendAlbum(std::move(album));
						m_albums.append(std::move(album));
				}

				m_artists.append(std::move(artist));
		}
}
