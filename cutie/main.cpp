#include <iostream>

#include <QApplication>

#include "ui/CpWindow.h"
#include "socksify/Socksifier.h"

int main(int argc, char** argv) {
	QApplication app(argc, argv);

	CpWindow mainWindow{ };
	mainWindow.show();

	return app.exec();
}