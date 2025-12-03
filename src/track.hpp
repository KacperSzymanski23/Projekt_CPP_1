#ifndef TRACK_HPP
#define TRACK_HPP

#include <QPixmap>
#include <QString>

class Track {
	  public:
		explicit Track(
			const uint32_t NUMBER = {}, QString title = {}, QString album = {}, QString artist = {}, QString duration = {}, const uint32_t YEAR = {},
			QString bitrate = {}, QString fileSize = {}, QPixmap cover = {}, QString path = {}
		);

		// Metadane z pliku audio
		uint32_t number;
		QString title;
		QString album;
		QString artist;
		QString duration;
		uint32_t year;
		QString bitrate;
		QString fileSize;
		QPixmap cover;

		QString path; // Ścierzka do pliku audio
};

#endif /* TRACK_HPP */