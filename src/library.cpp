#include "library.hpp"
// Qt
#include <QDir>
#include <QDirListing>
#include <QFileInfo>
#include <QFuture>
#include <QtConcurrent>
// STD
#include <algorithm>
// TagLib
#include <taglib/tpropertymap.h>
// Tracy
#include <tracy/Tracy.hpp>

Library::Album::Album(const QString &title, const QString &coverArtPath, const QList<TrackMetadata> &tracks, const QList<QString> &paths)
	: Collection(title, tracks)
	, m_coverArtPath(coverArtPath)
	, m_tracksPaths(paths) {
		ZoneScoped;

		findCoverArt(m_coverArtPath);
}

void Library::Album::setCoverArtPath(const QString &path) {
		m_coverArtPath = path;
}

QString Library::Album::getCoverArtPath() const {
		return m_coverArtPath;
}

void Library::Album::setData(const QList<TrackMetadata> &tracks, const QList<QString> &paths) {
		ZoneScoped;

		p_items = tracks;
		m_tracksPaths = paths;

		findCoverArt(m_coverArtPath);
}

void Library::Album::appendData(const TrackMetadata &track, const QString &path) {
		ZoneScoped;

		p_items.append(track);
		m_tracksPaths.append(path);
}

QString Library::Album::getPath(qsizetype index) const {
		if (index < m_tracksPaths.size()) {
				return m_tracksPaths.at(index);
		}

		return {};
}

QList<QString> Library::Album::getTracksPathsList() const {
		ZoneScoped;

		return m_tracksPaths;
}

void Library::Album::findCoverArt(const QString &path) {
		ZoneScoped;

		const QStringList COVER_FILES = {"cover.jpg", "cover.jpeg", "cover.png", "cover.webp"}; // Wspierane typy plików dla grafiki okładki

		const QDir DIRECTORY{path};
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
		p_items.append(album);
}

Library::Library(const QString &libraryPath)
	: m_libraryPath(libraryPath) {
}

void Library::setLibraryPath(const QString &path) {
		m_libraryPath = path;
}

QPair<Library::TrackMetadata, QString> Library::extractMetadata(const QString &path) {
		ZoneScoped;

		if (path.isEmpty()) {
				return {};
		}

		const QByteArray ENCODED_PATH = QFile::encodeName(path);
		const TagLib::FileRef FILE_REF(ENCODED_PATH.constData(), true, TagLib::AudioProperties::Fast);

		if (FILE_REF.isNull() || FILE_REF.tag() == nullptr) {
				return {};
		}

		const TagLib::Tag *TAG{FILE_REF.tag()};
		const TagLib::AudioProperties *AUDIO_PROPERTIES{FILE_REF.audioProperties()};
		const TagLib::PropertyMap PROPERTIES = FILE_REF.file()->properties();

		const uint32_t NUMBER{TAG->track()};
		const QString TITLE{QString::fromStdWString(TAG->title().toWString())};
		const QString ALBUM{QString::fromStdWString(TAG->album().toWString())};
		QString artist{QString::fromStdWString(TAG->artist().toWString())};
		const uint32_t YEAR{TAG->year()};

		const int32_t DURATION_IN_SECONDS{AUDIO_PROPERTIES->lengthInSeconds()};
		const int32_t BITRATE{AUDIO_PROPERTIES->bitrate()};

		const QTime DURATION{0, DURATION_IN_SECONDS / 60, DURATION_IN_SECONDS % 60};

		constexpr double MIB = 1024.0 * 8.0;
		const double FILE_SIZE = (DURATION_IN_SECONDS * BITRATE) / MIB;

		if (PROPERTIES.contains("ALBUMARTIST")) {
				artist = QString::fromStdWString(PROPERTIES["ALBUMARTIST"].front().toWString());
		}

		// path jest wogóle niepotrzebny ponieważ obecną ścieżkę można uzyskać
		// z m_queue.currentMedia()
		Library::TrackMetadata metadata = {
			.number = NUMBER,
			.title = TITLE,
			.album = ALBUM,
			.artist = artist,
			.duration = DURATION.toString("mm:ss"),
			.year = YEAR,
			.bitrate = QString::number(BITRATE) + " kbps",
			.fileSize = QString::number(FILE_SIZE, 'f', 1) + " MiB",
			.path = path,
		};

		if (metadata.title.isEmpty()) {
				metadata.title = path.split("/").last();
		}

		if (metadata.artist.isEmpty()) {
				metadata.artist = "Unknown";
		}

		if (metadata.album.isEmpty()) {
				metadata.album = "Unknown";
		}

		return {metadata, path};
}

void Library::scanLibraryPath() {
		ZoneScoped;

		if (m_libraryPath.isEmpty()) {
				m_libraryPath = QDir::homePath() + "/Music";
		}
		const QDir LIBRARY{m_libraryPath};

		if (!LIBRARY.exists()) {
				return;
		}

		constexpr auto FLAGS = QDirListing::IteratorFlag::Recursive | QDirListing::IteratorFlag::FilesOnly;
		const QStringList AUDIO_FILE_FILTER = {"*.mp3", "*.flac", "*.wav", "*.ogg", "*.opus", "*.m4a", "*.mka"}; // Wspierane typy plików

		QList<QString> tracksPaths{};
		tracksPaths.reserve(100);

		for (const auto &file : QDirListing(LIBRARY.path(), AUDIO_FILE_FILTER, FLAGS)) {
				tracksPaths.append(file.absoluteFilePath());
		}

		QFuture<std::pair<TrackMetadata, QString>> future = QtConcurrent::mapped(tracksPaths, extractMetadata);
		future.waitForFinished();

		auto results = future.results();
		groupTracks(results);
}

QList<Library::Artist> Library::getArtistList() const {
		ZoneScoped;

		return m_artists;
}

QList<Library::Album> Library::getAlbumsList() const {
		ZoneScoped;

		return m_albums;
}

Library::Album Library::getAlbum(qsizetype index) const {
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

Library::Artist Library::getArtist(qsizetype index) const {
		ZoneScoped;

		if (index < m_artists.size()) {
				return m_artists.at(index);
		}

		return Artist(nullptr);
}

void Library::groupTracks(QList<std::pair<TrackMetadata, QString>> &data) {
		ZoneScoped;

		if (data.isEmpty()) {
				return;
		}

		std::ranges::sort(data, [](const std::pair<TrackMetadata, QString> &left, const std::pair<TrackMetadata, QString> &right) {
				const auto &leftMetadata = left.first;
				const auto &rightMetadata = right.first;

				if (leftMetadata.artist != rightMetadata.artist) {
						return leftMetadata.artist < rightMetadata.artist;
				}
				if (leftMetadata.album != rightMetadata.album) {
						return leftMetadata.album < rightMetadata.album;
				}
				return leftMetadata.number < rightMetadata.number;
		});

		m_artists.reserve(10);
		m_albums.reserve(30);

		std::unique_ptr<Artist> currentArtist = nullptr;
		std::unique_ptr<Album> currentAlbum = nullptr;

		for (const auto &[meta, path] : data) {

				// Sprawdza czy currentArtist jest nullptr lub meta.artist jest rózny od nazwy currentArtist
				// jeśli warunek jest spełniony to oznacza to że ten artysta nie znajduje się w liście m_artists
				// lub currentArtist jest pusty
				if (currentArtist == nullptr || meta.artist != currentArtist->getName()) {
						// Jeśli currentAlbum niejst pusty to dodaj go do listy albumów w currentArtist
						if (currentAlbum != nullptr) {
								currentArtist->appendAlbum(*currentAlbum);
								m_albums.append(*currentAlbum);

								currentAlbum.reset();
						}
						// Jeśli currentArtist nie jest pusty to dodaj go do listy artystów
						if (currentArtist != nullptr) {
								m_artists.append(*currentArtist);
						}

						// Tworzenie nowego obiektu Artist i ustawienie go jako nowy currentArtist
						currentArtist = std::make_unique<Artist>(meta.artist);
				}

				// Sprawdza czy currentAlbum jest nullptr lub meta.album jest rózny od nazwy currentAlbum
				// jeśli warunek jest spełniony to oznacza to że ten album nie znajduje się w liście m_albums
				// lub currentAlbum jest pusty
				if (currentAlbum == nullptr || meta.album != currentAlbum->getName()) {
						if (currentAlbum != nullptr) {
								currentArtist->appendAlbum(*currentAlbum);
								m_albums.append(*currentAlbum);
						}

						// Tworzenie nowego obiektu Album i ustawienie go jako nowy currentAlbum
						QString dirPath = QFileInfo(path).path();
						currentAlbum = std::make_unique<Album>(meta.album, dirPath);
				}

				// Dodanie obiektu TrackMetadata do obiektu Album
				currentAlbum->appendData(meta, path);
		}

		// Dodanie ostatniego albumu do listy albumów w currentArtist
		if (currentAlbum != nullptr && currentArtist != nullptr) {
				currentArtist->appendAlbum(*currentAlbum);
				m_albums.append(*currentAlbum);
		}
		// Dodanie ostatniego artysty do listy artystów
		if (currentArtist != nullptr) {
				m_artists.append(*currentArtist);
		}
}
