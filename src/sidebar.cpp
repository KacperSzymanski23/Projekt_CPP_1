#include "sidebar.h"
#include "icons.hpp"
// Tracy
#include <tracy/Tracy.hpp>

SideBar::SideBar(QWidget *parent)
	: QWidget(parent)
	, m_sideBarLayout(new QVBoxLayout(this))
	, m_settingsDialog(new SettingsDialog(this))
	, m_showLibraryButton(new QToolButton(this))
	, m_showAlbumsButton(new QToolButton(this))
	, m_showPlaylistsButton(new QToolButton(this))
	, m_showFavoriteButton(new QToolButton(this))
	, m_settingsButton(new QToolButton(this)) {
		ZoneScoped;

		constexpr QSize ICON_SIZE = QSize(20, 20);

		// Tworzenie przycisku do wyswietlenia biblioteki
		m_showLibraryButton->setIcon(Icons::LIBRARY);
		m_showLibraryButton->setIconSize(ICON_SIZE);
		m_showLibraryButton->setStatusTip(tr("Library"));

		// Tworzenie przycisku do wyswietlenia albumow
		m_showAlbumsButton->setIcon(Icons::ALBUMS);
		m_showAlbumsButton->setIconSize(ICON_SIZE);
		m_showAlbumsButton->setStatusTip(tr("Albums"));

		// Tworzenie przycisku do wyswietlenia playlist
		m_showPlaylistsButton->setIcon(Icons::PLAYLIST);
		m_showPlaylistsButton->setIconSize(ICON_SIZE);
		m_showPlaylistsButton->setStatusTip(tr("Playlists"));

		// Tworzenie przycisku do wyswietlenia ulubionych utworów
		m_showFavoriteButton->setIcon(Icons::FAVORITE);
		m_showFavoriteButton->setIconSize(ICON_SIZE);
		m_showFavoriteButton->setStatusTip(tr("Favorite"));

		// Tworzenie przycisku do wyswietlenia okna dialogowego ustawień programu
		m_settingsButton->setIcon(Icons::SETTINGS);
		m_settingsButton->setIconSize(ICON_SIZE);
		m_settingsButton->setStatusTip(tr("Settings"));

		// Połańczenie sygnalów z slotami
		connect(m_showLibraryButton, &QToolButton::clicked, this, &SideBar::showLibraryClicked);
		connect(m_showAlbumsButton, &QToolButton::clicked, this, &SideBar::showAlbumsClicked);
		connect(m_showPlaylistsButton, &QToolButton::clicked, this, &SideBar::showPlaylistsClicked);
		connect(m_showFavoriteButton, &QToolButton::clicked, this, &SideBar::showFavoriteClicked);
		connect(m_settingsDialog, &SettingsDialog::settingsChanged, this, &SideBar::settingsChanged);

		connect(m_settingsButton, &QToolButton::clicked, m_settingsDialog, [this]() {
				m_settingsDialog->show();
				m_settingsDialog->raise();
				m_settingsDialog->activateWindow();
		});

		// Dodawanie przycisków do układu paska bocznego
		m_sideBarLayout->addWidget(m_showLibraryButton);
		m_sideBarLayout->addWidget(m_showAlbumsButton);
		m_sideBarLayout->addWidget(m_showPlaylistsButton);
		m_sideBarLayout->addWidget(m_showFavoriteButton);

		// Dodanie elementu strechera do układu
		m_sideBarLayout->addStretch();

		m_sideBarLayout->addWidget(m_settingsButton);
}
