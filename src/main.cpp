#include "mainwindow.hpp"
// Qt
#include <QApplication>

int32_t main(int32_t argc, char *argv[]) {
		QApplication app(argc, argv);

		MainWindow mainWin;

		mainWin.show();
		return QApplication::exec();
}