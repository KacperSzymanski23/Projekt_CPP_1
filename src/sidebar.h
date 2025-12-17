
#ifndef SIDEBAR_HPP
#define SIDEBAR_HPP

#include "settingsdialog.hpp"
// Qt
#include <QWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QSpacerItem>

class SideBar : public QWidget {
	Q_OBJECT

  public:
	explicit SideBar(QWidget *parent = nullptr);

signals:
	void showLibraryClicked();
	void showPlaylistsClicked();
	void showFavoriteClicked();
	void showAuthorsClicked();
	void showAlbumsClicked();
	void settingsClicked();
	void settingsChanged();
private:
	QVBoxLayout *m_sideBarLayout;  // Układ elementów GUI dla paska bocznego

	SettingsDialog *m_settingsDialog; // Okno zawierające ustawiania programu

	QToolButton *m_showLibraryButton;
	QToolButton *m_showPlaylistsButton;
	QToolButton *m_showFavoriteButton;
	QToolButton *m_showAuthorsButton;
	QToolButton *m_showAlbumsButton;

	QToolButton *m_settingsButton; // Przycisk optwierający ustawienia programu
};

#endif // SIDEBAR_HPP