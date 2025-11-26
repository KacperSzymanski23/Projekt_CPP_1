#include "mainwindow.hpp"
// Qt
#include <QApplication>
// Qlementine
#include <oclero/qlementine.hpp>
// Qlementine Icons
#include <oclero/qlementine/icons/QlementineIcons.hpp>

int32_t main(int32_t argc, char *argv[]) {
		// Objekt dla głównej klasy zarządzajacej GUI programu
		QApplication app(argc, argv);

		// Objekt okna programu
		MainWindow mainWin;

		// Objekt motywu programu
		auto* style = new oclero::qlementine::QlementineStyle(&app);

		style->setAnimationsEnabled(true);
		style->setAutoIconColor(oclero::qlementine::AutoIconColor::TextColor);
		style->setIconPathGetter(oclero::qlementine::icons::fromFreeDesktop);

		// Zamian domyślego motywu programu na Qlementine
		QApplication::setStyle(style);

		// Inicjalizacjalizuje i ustawia Qlementine jako domyślny motyw ikon
		oclero::qlementine::icons::initializeIconTheme();
		QIcon::setThemeName("qlementine");

		// Objekt menadżera motywów, który pozwala wczytć nasz własny styl z pliku json
		auto* themeManager = new oclero::qlementine::ThemeManager(style);
		themeManager->loadDirectory(":/themes");

		// Domyślny motyw programu będzi ustaiony na Jasny
		themeManager->setCurrentTheme("Light");

		mainWin.show();
		return QApplication::exec();
}