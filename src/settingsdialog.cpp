#include "settingsdialog.hpp"
#include "icons.hpp"
// Qt
#include <QFileDialog>
// Tracy
#include <tracy/Tracy.hpp>

SettingsDialog::SettingsDialog(QWidget *parent)
	: QDialog(parent)
	, m_dialogGrigLayout(new QGridLayout(this))
	, m_settings(Settings::getInstance())
	, m_libraryDirectoryLabel(new QLabel(this))
	, m_libraryDirectoryLineEdit(new QLineEdit(this))
	, m_browserFilesButton(new QPushButton(this))
	, m_saveButton(new QPushButton(this))
	, m_cancelButton(new QPushButton(this))
	, m_applyButton(new QPushButton(this)) {
		ZoneScoped;

		m_libraryDirectoryLabel->setText("Library path");

		const QString LINE_EDIT_TEXT = QString::fromStdString(m_settings.getSettingsEntry("libraryDirectory"));
		m_libraryDirectoryLineEdit->setText(LINE_EDIT_TEXT);
		m_browserFilesButton->setIcon(Icons::FOLDER);

		m_applyButton->setDisabled(true);

		m_saveButton->setIcon(Icons::SAVE);
		m_cancelButton->setIcon(Icons::CANCEL);
		m_applyButton->setIcon(Icons::APPLY);

		m_saveButton->setText("Save");
		m_cancelButton->setText("Cancel");
		m_applyButton->setText("Apply");

		connect(m_browserFilesButton, &QPushButton::clicked, this, [this]() {
				m_libraryDirectory = QFileDialog::getExistingDirectory(this, "Wybierz lokalizację biblioteki muzycznej", QDir::homePath());
				m_libraryDirectoryLineEdit->setText(m_libraryDirectory.absolutePath());
		});

		connect(m_libraryDirectoryLineEdit, &QLineEdit::textChanged, this, [this]() { m_applyButton->setDisabled(false); });

		connect(m_saveButton, &QPushButton::clicked, this, [this]() {
				applySettings();
				close();
		});
		connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::close);
		connect(m_applyButton, &QPushButton::clicked, this, &SettingsDialog::applySettings);

		m_dialogGrigLayout->addWidget(m_libraryDirectoryLabel, 0, 0, 1, 1);
		m_dialogGrigLayout->addWidget(m_libraryDirectoryLineEdit, 0, 1, 1, 1);
		m_dialogGrigLayout->addWidget(m_browserFilesButton, 0, 2, 1, 1);
		m_dialogGrigLayout->addWidget(m_saveButton, 1, 2, 1, 1);
		m_dialogGrigLayout->addWidget(m_cancelButton, 1, 3, 1, 1);
		m_dialogGrigLayout->addWidget(m_applyButton, 1, 4, 1, 1);

		this->setLayout(m_dialogGrigLayout);
}

void SettingsDialog::applySettings() {
		ZoneScoped;

		// libraryDirectory
		const std::string LIBRARY_DIRECTORY{m_libraryDirectory.absolutePath().toStdString()};

		m_settings.setSettingsEntry("libraryDirectory", LIBRARY_DIRECTORY);
		m_settings.saveSettings();

		m_applyButton->setDisabled(true);

		emit settingsChanged();
}