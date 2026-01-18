#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include "settings.hpp"
// Qt
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class SettingsDialog : public QDialog {
		Q_OBJECT

	  public:
		explicit SettingsDialog(QWidget *parent = nullptr);

	  private:
		QGridLayout *m_dialogGrigLayout; // Układ elementów GUI dla okna dialogowego

		Settings &m_settings; // Referencja do obiektu Settings

		// Inne elementy GUI zwiazane z zmianą ścieżki do biblioteki
		QLabel *m_libraryDirectoryLabel;
		QLineEdit *m_libraryDirectoryLineEdit;
		QPushButton *m_browserFilesButton;
		QDir m_libraryDirectory;

		// Inne elementy GUI zwiazane z zmianą motywu programu
		QLabel *m_themeLabel;
		QComboBox *m_themeComboBox;

		// Przyciski do zapisu, anulowania i zastosowania ustawienia
		QPushButton *m_saveButton;
		QPushButton *m_cancelButton;
		QPushButton *m_applyButton;

	  private slots:
		void applySettings(); // Zapisywanie ustawienia

	  signals:
		void settingsChanged(); // Sygnał emitowany przy zapisaniu ustawienia
};

#endif /* SETTINGSDIALOG_HPP */