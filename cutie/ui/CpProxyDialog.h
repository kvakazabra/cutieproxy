#pragma once
#include <QDialog>

#include "../core/QProxyItem.h"

namespace Ui {
	class ProxyDialog;
}

class CpProxyDialog : public QDialog {
public:
	CpProxyDialog(QWidget* parent, QProxyItem* prototype = nullptr);
	~CpProxyDialog();

	QProxyItem* proxyItem();
private:
	void init();
	void connectSignals();

private:
	Ui::ProxyDialog* m_Ui{ };
	const QProxyItem* m_Prototype{ };
};