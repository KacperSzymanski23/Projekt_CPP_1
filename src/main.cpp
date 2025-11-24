#include "mainwindow.hpp"
// Qt
#include <QApplication>
// Qlementine
#include <oclero/qlementine.hpp>
// Qlementine Icons
#include <oclero/qlementine/icons/QlementineIcons.hpp>

int32_t main(int32_t argc, char *argv[]) {
		QApplication app(argc, argv);

		MainWindow mainWin;

		auto* style = new oclero::qlementine::QlementineStyle(&app);

		style->setAnimationsEnabled(true);
		style->setAutoIconColor(oclero::qlementine::AutoIconColor::TextColor);
		style->setIconPathGetter(oclero::qlementine::icons::fromFreeDesktop);

		QApplication::setStyle(style);

		oclero::qlementine::icons::initializeIconTheme();
		QIcon::setThemeName("qlementine");

		auto* themeManager = new oclero::qlementine::ThemeManager(style);
		themeManager->loadDirectory(":/themes");

		themeManager->setCurrentTheme("Light");

		mainWin.show();
		return QApplication::exec();
}