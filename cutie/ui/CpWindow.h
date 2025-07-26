#pragma once
#include <memory>

#include <QMainWindow>

#include "../core/Socksifier.h"

namespace Ui {
	class MainWindow;
}

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
	std::unique_ptr<Socksifier> m_Socksifier{ };
	Ui::MainWindow* m_Ui{ };

	bool m_Started{ };
};