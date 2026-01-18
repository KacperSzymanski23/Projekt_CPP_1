
#ifndef SIDEBAR_HPP
#define SIDEBAR_HPP

#include "settingsdialog.hpp"
// Qt
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

class SideBar : public QWidget {
		Q_OBJECT

	  public:
		explicit SideBar(QWidget *parent = nullptr);

	  signals:
		// Sygnały emitowane przez przyciski paska bocznego
		void showLibraryClicked();
		void showAlbumsClicked();
		void showPlaylistsClicked();
		void showFavoriteClicked();
		void settingsClicked();
		void settingsChanged();

	  private:
		QVBoxLayout *m_sideBarLayout; // Układ elementów GUI dla paska bocznego

		SettingsDialog *m_settingsDialog; // Okno zawierające ustawiania programu

		// Przyciski paska bocznego
		QToolButton *m_showLibraryButton;
		QToolButton *m_showAlbumsButton;
		QToolButton *m_showPlaylistsButton;
		QToolButton *m_showFavoriteButton;

		QToolButton *m_settingsButton; // Przycisk optwierający ustawienia programu
};

#endif // SIDEBAR_HPP