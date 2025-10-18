#ifndef TRACK_HPP
#define TRACK_HPP

#include <QPixmap>
#include <QString>

class Track {
	  public:
		explicit Track(
			uint32_t number = {}, QString title = {}, QString album = {}, QString artist = {}, QString duration = {}, uint32_t year = {},
			QString bitrate = {}, QString fileSize = {}, QPixmap cover = {}, QString path = {}
		);
		~Track();

		uint32_t number;
		QString title;
		QString album;
		QString artist;
		QString duration;
		uint32_t year;
		QString bitrate;
		QString fileSize;
		QPixmap cover;
		QString path;
};

#endif /* TRACK_HPP */