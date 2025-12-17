#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include "settings.hpp"
// Qt
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
		QGridLayout *m_dialogGrigLayout;

		Settings m_settings;

		QLabel *m_libraryDirectoryLabel;
		QLineEdit *m_libraryDirectoryLineEdit;
		QPushButton *m_browserFilesButton;
		QDir m_libraryDirectory;

		QPushButton *m_saveButton;
		QPushButton *m_cancelButton;
		QPushButton *m_applyButton;

	  private slots:
		void applySettings();

	signals:
	void settingsChanged();
};

#endif /* SETTINGSDIALOG_HPP */