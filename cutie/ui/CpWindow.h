#pragma once
#include <memory>

#include <QMainWindow>

#include "ui_CpWindow.h"

class CpWindow : public QMainWindow {
	Q_OBJECT
public:
	CpWindow();
	~CpWindow();

private:
	void init();
	void connectSignals();
private:
	Ui_MainWindow* m_Ui{ };
};