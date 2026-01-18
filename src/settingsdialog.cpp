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
	, m_themeLabel(new QLabel(this))
	, m_themeComboBox(new QComboBox(this))
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
				// Wybieranie lokalizacji biblioteki
				m_libraryDirectory = QFileDialog::getExistingDirectory(this, "Wybierz lokalizację biblioteki muzycznej", QDir::homePath());
				m_libraryDirectoryLineEdit->setText(m_libraryDirectory.absolutePath());
		});

		// Gdy zmieni się tekst w QLineEdit odblokuj przycisk Apply
		connect(m_libraryDirectoryLineEdit, &QLineEdit::textChanged, this, [this]() { m_applyButton->setDisabled(false); });

		connect(m_saveButton, &QPushButton::clicked, this, [this]() {
				applySettings();
				close();
		});
		connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::close);
		connect(m_applyButton, &QPushButton::clicked, this, &SettingsDialog::applySettings);

		m_themeLabel->setText("Choose theme");

		m_themeComboBox->addItem("Dark");
		m_themeComboBox->addItem("Light");

		m_themeComboBox->setCurrentIndex(m_settings.getSettingsEntry("theme") == "Dark" ? 0 : 1);

		// Dodawanie elementów do m_dialogGrigLayout
		m_dialogGrigLayout->addWidget(m_libraryDirectoryLabel, 0, 0, 1, 1);
		m_dialogGrigLayout->addWidget(m_libraryDirectoryLineEdit, 0, 1, 1, 1);
		m_dialogGrigLayout->addWidget(m_browserFilesButton, 0, 2, 1, 1);

		m_dialogGrigLayout->addWidget(m_themeLabel, 1, 0, 1, 1);
		m_dialogGrigLayout->addWidget(m_themeComboBox, 1, 1, 1, 1);

		m_dialogGrigLayout->addWidget(m_saveButton, 2, 2, 1, 1);
		m_dialogGrigLayout->addWidget(m_cancelButton, 2, 3, 1, 1);
		m_dialogGrigLayout->addWidget(m_applyButton, 2, 4, 1, 1);

		this->setLayout(m_dialogGrigLayout);
}

void SettingsDialog::applySettings() {
		ZoneScoped;

		// libraryDirectory
		const std::string LIBRARY_DIRECTORY{m_libraryDirectory.absolutePath().toStdString()};
		const std::string THEME{m_themeComboBox->currentText().toStdString()};

		m_settings.setSettingsEntry("libraryDirectory", LIBRARY_DIRECTORY);
		m_settings.setSettingsEntry("theme", THEME);
		m_settings.saveSettings();

		m_applyButton->setDisabled(true);

		emit settingsChanged();
}