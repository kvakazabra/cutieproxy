#include <iostream>

#include <QApplication>

#include "ui/CpWindow.h"
#include "core/Socksifier.h"

int main(int argc, char** argv) {
	QApplication app(argc, argv);

	CpWindow mainWindow{ };
	mainWindow.show();

	return app.exec();
}