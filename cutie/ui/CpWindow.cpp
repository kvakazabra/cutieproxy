#include "CpWindow.h"

#include <QObject>

CpWindow::CpWindow() 
	: m_Ui{ new Ui_MainWindow() } {
	m_Ui->setupUi(this);
}

CpWindow::~CpWindow() {
	delete m_Ui;
	m_Ui = { };
}

void CpWindow::init() {

}

void CpWindow::connectSignals() {

	//QObject::connect()
}