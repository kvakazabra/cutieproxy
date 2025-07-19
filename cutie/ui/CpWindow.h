#pragma once
#include <memory>

#include <QMainWindow>

#include "../core/Socksifier.h"
#include "ui_CpWindow.h"

class CpWindow : public QMainWindow {
	Q_OBJECT
public:
	CpWindow();
	~CpWindow();

private:
	void init();
	void connectSignals();

	void processProxyItems();
	void settingsChanged();
private Q_SLOTS:
	void addProxyItem();
private:
	//Socksifier* m_Socksifier{ };
	std::unique_ptr<Socksifier> m_Socksifier{ };
	Ui_MainWindow* m_Ui{ };

	bool m_Started{ };
};