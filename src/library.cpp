#include "library.hpp"
#include "logs.hpp"
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

		logCreate("Index out of range: " + std::to_string(index));

		return {};
}

QList<QString> Library::Album::getTracksPathsList() const {
		ZoneScoped;

		return m_tracksPaths;
}

void Library::Album::findCoverArt(const QString &path) {
		ZoneScoped;

		const QStringList COVER_FILES = {"cover.jpg", "cover.jpeg", "cover.png", "cover.webp"}; // Lista plików okładek albumu

		const QDir DIRECTORY{path};
		for (const QString &fileName : COVER_FILES) {
				const QFileInfo FILE_INFO{DIRECTORY, fileName};

				// Jeśli plik istnieje i jest plikiem
				if (FILE_INFO.exists() && FILE_INFO.isFile()) {
						m_coverArtPath = FILE_INFO.absoluteFilePath();

						return;
				}
		}

		m_coverArtPath = ":/Placeholder";

		logCreate("Cover art not found");
}

Library::Artist::Artist(const QString &name, const QList<Album> &albums)
	: Collection(name, albums) {
}

Library::Album Library::Artist::findAlbum(const QString &title) const {
		ZoneScoped;

		// Szuka albumu o podanej nazwie
		for (const auto &album : p_items) {
				if (album.getName() == title) {
						return album;
				}
		}

		logCreate("Album not found: " + title.toStdString());

		return {};
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

		// Sprawdza czy ścieżka jest pusta
		if (path.isEmpty()) {
				logCreate("Path is empty");
				return {};
		}

		// Enkodowanie ścieżki do QByteArray
		const QByteArray ENCODED_PATH = QFile::encodeName(path);

		// Otwiera plik audio i pobiera metadane
		const TagLib::FileRef FILE_REF(ENCODED_PATH.constData(), true, TagLib::AudioProperties::Fast);

		// Sprawdza czy plik jest pusty lub nie ma metadanych
		if (FILE_REF.isNull() || FILE_REF.tag() == nullptr) {
				logCreate("FileRef is null");
				return {};
		}

		// Pobiera metadane i inne informacje z pliku audio
		const TagLib::Tag *tag{FILE_REF.tag()};
		const TagLib::AudioProperties *audioProperties{FILE_REF.audioProperties()};
		const TagLib::PropertyMap PROPERTIES = FILE_REF.file()->properties();

		// Konwertuje metadane do odpowiednich typów
		const uint32_t NUMBER{tag->track()};
		const QString TITLE{QString::fromStdWString(tag->title().toWString())};
		const QString ALBUM{QString::fromStdWString(tag->album().toWString())};
		QString artist{QString::fromStdWString(tag->artist().toWString())};
		const uint32_t YEAR{tag->year()};

		const int32_t DURATION_IN_SECONDS{audioProperties->lengthInSeconds()};
		const int32_t BITRATE{audioProperties->bitrate()};

		// Tworzy obiekt QTime z czasem trwania piosenki
		const QTime DURATION{0, DURATION_IN_SECONDS / 60, DURATION_IN_SECONDS % 60};

		constexpr double MIB = 1024.0 * 8.0;
		// Oblicza rozmiar pliku
		const double FILE_SIZE = (DURATION_IN_SECONDS * BITRATE) / MIB;

		// Sprawdza czy istnieje własność "ALBUMARTIST"
		if (PROPERTIES.contains("ALBUMARTIST")) {
				// Ustawia artystę na "ALBUMARTIST"
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

		// Sprawdza czy nazwa utworu jest pusta
		if (metadata.title.isEmpty()) {
				// Ustawia nazwę utworu na nazwę pliku
				metadata.title = path.split("/").last();
		}

		if (metadata.artist.isEmpty()) {
				logCreate("Artist name is empty");
				metadata.artist = "Unknown";
		}

		if (metadata.album.isEmpty()) {
				logCreate("Album name is empty");
				metadata.album = "Unknown";
		}

		return {metadata, path};
}

void Library::scanLibraryPath() {
		ZoneScoped;

		m_albums.clear();
		m_artists.clear();

		if (m_libraryPath.isEmpty()) {
				logCreate("Library path is empty, using default path");
				m_libraryPath = QDir::homePath() + "/Music";
		}

		const QDir LIBRARY{m_libraryPath};

		if (!LIBRARY.exists()) {
				logCreate("Library path does not exists");
				return;
		}

		constexpr auto FLAGS = QDirListing::IteratorFlag::Recursive | QDirListing::IteratorFlag::FilesOnly;      // Flagi QDirListing
		const QStringList AUDIO_FILE_FILTER = {"*.mp3", "*.flac", "*.wav", "*.ogg", "*.opus", "*.m4a", "*.mka"}; // Wspierane typy plików

		QList<QString> tracksPaths{};
		tracksPaths.reserve(100);

		// Przeszukuję katalog biblioteki w poszukiwaniu plików audio
		for (const auto &file : QDirListing(LIBRARY.path(), AUDIO_FILE_FILTER, FLAGS)) {
				tracksPaths.append(file.absoluteFilePath());
		}

		// Wykonuje operację pobierania metadanych z plików na wielu wątkach
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

		// Sprawdza czy indeks jest w zakresie
		if (index < m_albums.size()) {
				return m_albums.at(index);
		}

		logCreate("Index out of range: " + std::to_string(index));

		return {};
}

Library::Artist Library::getArtist(const QString &name) const {
		ZoneScoped;

		// Szuka artysty o podanej nazwie
		for (const auto &artist : m_artists) {
				if (artist.getName() == name) {
						return artist;
				}
		}

		logCreate("Artist not found: " + name.toStdString());

		return Artist(nullptr);
}

Library::Artist Library::getArtist(qsizetype index) const {
		ZoneScoped;

		// Sprawdza czy indeks jest w zakresie
		if (index < m_artists.size()) {
				return m_artists.at(index);
		}

		logCreate("Index out of range: " + std::to_string(index));

		return Artist(nullptr);
}

void Library::groupTracks(QList<std::pair<TrackMetadata, QString>> &data) {
		ZoneScoped;

		if (data.isEmpty()) {
				logCreate("Data is empty");
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
