#include "sidebar.h"
#include "icons.hpp"
// Tracy
#include <tracy/Tracy.hpp>

SideBar::SideBar(QWidget *parent)
	: QWidget(parent)
	, m_sideBarLayout(new QVBoxLayout(this))
	, m_settingsDialog(new SettingsDialog(this))
	, m_showLibraryButton(new QToolButton(this))
	, m_showPlaylistsButton(new QToolButton(this))
	, m_showFavoriteButton(new QToolButton(this))
	, m_showAuthorsButton(new QToolButton(this))
	, m_showAlbumsButton(new QToolButton(this))
	, m_settingsButton(new QToolButton(this)) {
		ZoneScoped;

		constexpr QSize ICON_SIZE = QSize(20, 20);

		m_showLibraryButton->setIcon(Icons::LIBRARY);
		m_showLibraryButton->setIconSize(ICON_SIZE);
		m_showLibraryButton->setStatusTip(tr("Library"));

		m_showPlaylistsButton->setIcon(Icons::PLAYLIST);
		m_showPlaylistsButton->setIconSize(ICON_SIZE);
		m_showPlaylistsButton->setStatusTip(tr("Playlists"));

		m_showFavoriteButton->setIcon(Icons::FAVORITE);
		m_showFavoriteButton->setIconSize(ICON_SIZE);
		m_showFavoriteButton->setStatusTip(tr("Favorite"));

		m_showAuthorsButton->setIcon(Icons::AUTHORS);
		m_showAuthorsButton->setIconSize(ICON_SIZE);
		m_showAuthorsButton->setStatusTip(tr("Authors"));

		m_showAlbumsButton->setIcon(Icons::ALBUMS);
		m_showAlbumsButton->setIconSize(ICON_SIZE);
		m_showAlbumsButton->setStatusTip(tr("Albums"));

		m_settingsButton->setIcon(Icons::SETTINGS);
		m_settingsButton->setIconSize(ICON_SIZE);
		m_settingsButton->setStatusTip(tr("Settings"));

		connect(m_showLibraryButton, &QToolButton::clicked, this, &SideBar::showLibraryClicked);
		connect(m_showPlaylistsButton, &QToolButton::clicked, this, &SideBar::showPlaylistsClicked);
		connect(m_showFavoriteButton, &QToolButton::clicked, this, &SideBar::showFavoriteClicked);
		connect(m_showAuthorsButton, &QToolButton::clicked, this, &SideBar::showAuthorsClicked);
		connect(m_showAlbumsButton, &QToolButton::clicked, this, &SideBar::showAlbumsClicked);
		connect(m_settingsDialog, &SettingsDialog::settingsChanged, this, &SideBar::settingsChanged);

		connect(m_settingsButton, &QToolButton::clicked, m_settingsDialog, [this]() {
				m_settingsDialog->show();
				m_settingsDialog->raise();
				m_settingsDialog->activateWindow();
		});

		m_sideBarLayout->addWidget(m_showLibraryButton);
		m_sideBarLayout->addWidget(m_showPlaylistsButton);
		m_sideBarLayout->addWidget(m_showFavoriteButton);
		m_sideBarLayout->addWidget(m_showAuthorsButton);
		m_sideBarLayout->addWidget(m_showAlbumsButton);

		m_sideBarLayout->addStretch();

		m_sideBarLayout->addWidget(m_settingsButton);
}
