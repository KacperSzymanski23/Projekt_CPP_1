#ifndef ICONS_HPP
#define ICONS_HPP

// Qt
#include <QIcon>
// Qlementine Icons
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/icons/QlementineIcons.hpp>

namespace Icons {
		using oclero::qlementine::icons::iconPath;
		using oclero::qlementine::icons::Icons16;

		// Ikony używane przez program
		const QIcon CANCEL = QIcon::fromTheme(iconPath(Icons16::Action_Close));
		const QIcon SAVE = QIcon::fromTheme(iconPath(Icons16::Action_Save));
		const QIcon FOLDER = QIcon::fromTheme(iconPath(Icons16::File_Folder));
		const QIcon APPLY = QIcon::fromTheme(iconPath(Icons16::Misc_Task));

		const QIcon LIBRARY = QIcon::fromTheme(iconPath(Icons16::Misc_Library));
		const QIcon PLAYLIST = QIcon::fromTheme(iconPath(Icons16::Media_Playlist));
		const QIcon FAVORITE = QIcon::fromTheme(iconPath(Icons16::Shape_HeartFilled));
		const QIcon AUTHORS = QIcon::fromTheme(iconPath(Icons16::Misc_Users));
		const QIcon ALBUMS = QIcon::fromTheme(iconPath(Icons16::File_FolderFilled));
		const QIcon SETTINGS = QIcon::fromTheme(iconPath(Icons16::Navigation_Settings));

		const QIcon LOOP = QIcon::fromTheme(iconPath(Icons16::Media_Loop));
		const QIcon SHUFFLE = QIcon::fromTheme(iconPath(Icons16::Media_Shuffle));
		const QIcon PREVIOUS = QIcon::fromTheme(iconPath(Icons16::Media_SeekBackward));
		const QIcon NEXT = QIcon::fromTheme(iconPath(Icons16::Media_SeekForward));
		const QIcon PLAY = QIcon::fromTheme(iconPath(Icons16::Media_Play));
		const QIcon PAUSE = QIcon::fromTheme(iconPath(Icons16::Media_Pause));
		const QIcon HIGH_VOLUME = QIcon::fromTheme(iconPath(Icons16::Audio_Speaker2));
		const QIcon MEDIUM_VOLUME = QIcon::fromTheme(iconPath(Icons16::Audio_Speaker1));
		const QIcon LOW_VOLUME = QIcon::fromTheme(iconPath(Icons16::Audio_Speaker0));
		const QIcon MUTE = QIcon::fromTheme(iconPath(Icons16::Audio_SpeakerMute));
		const QIcon NO_FAVORITE = QIcon::fromTheme(iconPath(Icons16::Shape_Heart));

} // namespace Icons

#endif /* ICONS_HPP */