#include "library.hpp"
// Qt
#include <QDir>
#include <QDirListing>
#include <QFileInfo>
#include <QFuture>
#include <QtConcurrent>
// Tracy
#include <tracy/Tracy.hpp>

Library::Album::Album(const QString &title, const QUrl &coverArtPath, const QList<TrackMetadata> &tracks, const QList<QUrl> &paths)
	: Collection(title, tracks)
	, m_coverArtPath(coverArtPath)
	, m_tracksPaths(paths) {
		ZoneScoped;

		findCoverArt(m_coverArtPath);
}

void Library::Album::setCoverArtPath(const QUrl &path) {
		m_coverArtPath = path;
}

QUrl Library::Album::getCoverArtPath() const {
		return m_coverArtPath;
}

void Library::Album::setData(const QList<TrackMetadata> &tracks, const QList<QUrl> &paths) {
		ZoneScoped;

		p_items = std::move(tracks);
		m_tracksPaths = std::move(paths);

		findCoverArt(m_coverArtPath);
}

void Library::Album::appendData(const TrackMetadata &track, const QUrl &path) {
		ZoneScoped;

		p_items.append(track);
		m_tracksPaths.append(path);
}

QUrl Library::Album::getUrl(uint32_t index) const {
		if (index < m_tracksPaths.size()) {
				return m_tracksPaths.at(index);
		}

		return {};
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
	: Collection(name, albums) {
}

Library::Album Library::Artist::findAlbum(const QString &title) const {
		ZoneScoped;

		for (const auto &album : p_items) {
				if (album.getName() == title) {
						return album;
				}
		}

		return Album();
}

void Library::Artist::appendAlbum(const Album &album) {
		p_items.append(std::move(album));
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
		const QDir LIBRARY{m_libraryPath.toString()};

		if (!LIBRARY.exists()) {
				return;
		}

		constexpr auto FLAGS = QDirListing::IteratorFlag::Recursive | QDirListing::IteratorFlag::FilesOnly;
		const QStringList AUDIO_FILE_FILTER = {"*.mp4", "*.mp3", "*.flac", "*.wav", "*.ogg", "*.opus", "*.m4a", "*.mka"}; // Wspierane typy plików

		QList<QUrl> tracksPaths;
		for (const auto &file : QDirListing(LIBRARY.path(), AUDIO_FILE_FILTER, FLAGS)) {
				tracksPaths.append(file.absoluteFilePath());
		}

		using Collector = QHash<QString, QHash<QString, QList<QPair<TrackMetadata, QUrl>>>>;

		auto mapData = [](const QUrl &filePath) -> QPair<TrackMetadata, QUrl> {
				ZoneScopedN("mapData");

				const QString FILE_PATH = filePath.toString();
				const QByteArray ENCODED_PATH = QFile::encodeName(FILE_PATH);
				const TagLib::FileRef FILE_REF(ENCODED_PATH.constData(), true, TagLib::AudioProperties::Fast);

				return {extractMetadata(FILE_PATH, FILE_REF), QUrl(FILE_PATH)};
		};

		auto reduce = [](Collector &collector, const QPair<TrackMetadata, QUrl> &item) {
				ZoneScopedN("reduce");

				const TrackMetadata &meta = item.first;
				collector[meta.artist][meta.album].append(item);
		};

		QFuture<Collector> future = QtConcurrent::mappedReduced(tracksPaths, mapData, reduce, QtConcurrent::UnorderedReduce);
		future.waitForFinished();

		groupTracks(future.result());
}

QList<Library::Artist> Library::getArtistList() const {
		ZoneScoped;

		return m_artists;
}

QList<Library::Album> Library::getAlbumsList() const {
		ZoneScoped;

		return m_albums;
}

Library::Album Library::getAlbum(uint32_t index) const {
		ZoneScoped;

		if (index < m_albums.size()) {
				return m_albums.at(index);
		}

		return {};
}

Library::Artist Library::getArtist(const QString &name) const {
		ZoneScoped;

		for (const auto &artist : m_artists) {
				if (artist.getName() == name) {
						return artist;
				}
		}

		return Artist(nullptr);
}

Library::Artist Library::getArtist(int32_t index) const {
		ZoneScoped;

		if (index < m_artists.size()) {
				return m_artists.at(index);
		}

		return Artist(nullptr);
}

void Library::groupTracks(const QHash<QString, QHash<QString, QList<QPair<TrackMetadata, QUrl>>>> &collector) {
		ZoneScoped;

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
