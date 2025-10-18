#include "track.hpp"

#include <utility>

Track::Track(
	uint32_t number, QString title, QString album, QString artist, QString duration, uint32_t year, QString bitrate, QString fileSize,
	QPixmap cover, QString path
)
	: number(number)
	, title(std::move(title))
	, album(std::move(album))
	, artist(std::move(artist))
	, duration(duration)
	, year(year)
	, bitrate(std::move(bitrate))
	, fileSize(std::move(fileSize))
	, cover(std::move(cover))
	, path(std::move(path)) {
}

Track::~Track() = default;