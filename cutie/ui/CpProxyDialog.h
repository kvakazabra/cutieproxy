#pragma once
#include <QDialog>

#include "../core/QProxyItem.h"
#include "ui_CpProxyDialog.h"

class CpProxyDialog : public QDialog {
public:
	CpProxyDialog(QWidget* parent, QProxyItem* prototype = nullptr);
	~CpProxyDialog();

	QProxyItem* proxyItem();
private:
	void init();
	void connectSignals();

private:
	Ui_Dialog* m_Ui{ };
	QProxyItem* m_Prototype{ };
};