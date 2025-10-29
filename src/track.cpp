#include "track.hpp"

#include <utility>

Track::Track(
	const uint32_t NUMBER, QString title, QString album, QString artist, QString duration, const uint32_t YEAR, QString bitrate,
	QString fileSize, QPixmap cover, QString path
)
	: number(NUMBER)
	, title(std::move(title))
	, album(std::move(album))
	, artist(std::move(artist))
	, duration(std::move(duration))
	, year(YEAR)
	, bitrate(std::move(bitrate))
	, fileSize(std::move(fileSize))
	, cover(std::move(cover))
	, path(std::move(path)) {
}

Track::~Track() = default;