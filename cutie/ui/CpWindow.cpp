#include "CpWindow.h"
#include "CpProxyDialog.h"
#include "ui_CpWindow.h"

#include "../core/QProxyItem.h"

#include <QObject>
#include <QHostAddress>
#include <QMessageBox>

CpWindow::CpWindow() 
	: m_Ui{ new Ui::MainWindow() } {
	m_Ui->setupUi(this);

	init();
	connectSignals();
}

CpWindow::~CpWindow() {
	delete m_Ui;
	m_Ui = { };
}

void CpWindow::init() {
	m_Ui->stopButton->setDisabled(true);

	m_Ui->proxyList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	m_Ui->proxyList->setTextElideMode(Qt::ElideNone);
	m_Ui->proxyList->setStyleSheet(
		"QListWidget::item {"
		"   padding: 10px 0px;"  // top/bottom padding of 10px, left/right of 0px
		"   border-bottom: 1px solid #eeeeee;"  // optional separator
		"}"
		"QListWidget::item:hover {"
		"   background-color: #aaaaaa;"
		"}"
		"QListWidget::item:selected {"
		"   background-color: #e0e0e0;"
		"   color: black;"
		"}"
	);
}

void CpWindow::connectSignals() {
	QObject::connect(m_Ui->addProxyButton, &QPushButton::clicked, this, &CpWindow::addProxyItem);

	QObject::connect(m_Ui->startButton, &QPushButton::clicked, [this]() -> void {
		if (m_Started) {
			return;
		}

		m_Started = true;

		m_Socksifier = std::make_unique<Socksifier>(TLogLevel::all);
		processProxyItems();
		m_Socksifier->start();

		m_Ui->stopButton->setEnabled(true);
		m_Ui->startButton->setDisabled(true);

		QMessageBox::information(this, QObject::tr("Info"), QObject::tr("Started!"), QMessageBox::StandardButton::Ok);
		});

	QObject::connect(m_Ui->stopButton, &QPushButton::clicked, [this]() -> void {
		if (!m_Started) {
			return;
		}

		m_Started = false;
		m_Socksifier->stop();
		m_Socksifier = { };

		m_Ui->startButton->setEnabled(true);
		m_Ui->stopButton->setDisabled(true);

		QMessageBox::information(this, QObject::tr("Info"), QObject::tr("Stopped!"), QMessageBox::StandardButton::Ok);
		});

	QObject::connect(m_Ui->proxyList, &QListView::customContextMenuRequested, [this](const QPoint& p) -> void {
		QPoint item = m_Ui->proxyList->mapToGlobal(p);

		QMenu submenu{ };
		auto editAction = submenu.addAction("Edit");
		auto deleteAction = submenu.addAction("Delete");

		QAction* selectedAction = submenu.exec(item);
		if (!selectedAction) {
			return;
		}

		QModelIndex modelIndex = m_Ui->proxyList->indexAt(p);

		if (selectedAction == editAction) {
			QProxyItem* item = dynamic_cast<QProxyItem*>(m_Ui->proxyList->item(modelIndex.row()));
			if (!item) {
				return;
			}

			CpProxyDialog dialog(this, item);
			dialog.show();

			QDialog::DialogCode r = static_cast<QDialog::DialogCode>(dialog.exec());
			if (r == QDialog::DialogCode::Rejected) {
				return;
			}

			QProxyItem* newItem = dialog.proxyItem();
			if (!newItem) {
				return;
			}

			m_Ui->proxyList->insertItem(modelIndex.row(), newItem);
			m_Ui->proxyList->takeItem(modelIndex.row() + 1);
		}
		else if (selectedAction == deleteAction) {
			m_Ui->proxyList->takeItem(modelIndex.row());

			settingsChanged();
		}
		});
}

void CpWindow::processProxyItems() {
	for (int i = 0; i < m_Ui->proxyList->count(); ++i) {
		QProxyItem* proxyItem = dynamic_cast<QProxyItem*>(m_Ui->proxyList->item(i));
		if (!proxyItem) {
			continue;
		}

		if (proxyItem->error() != TProxyErrorType::None) {
			continue;
		}

		std::optional<std::size_t> proxyId = m_Socksifier->addSocks5Proxy(proxyItem->endpoint().toStdString(),
			proxyItem->protocols(), true, proxyItem->username().toStdString(), proxyItem->password().toStdString());
		if (!proxyId.has_value()) {
			QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Failed registering a proxy!"), QMessageBox::StandardButton::Ok);
			continue;
		}

		for (auto& filter : proxyItem->filters()) {
			m_Socksifier->addFilterToProxy(proxyId.value(), filter);
		}
	}
}

void CpWindow::settingsChanged() {
	// alert user that restart is needed to apply them
}

void CpWindow::addProxyItem() {
	CpProxyDialog dialog(this);
	dialog.show();

	QDialog::DialogCode r = static_cast<QDialog::DialogCode>(dialog.exec());
	if (r == QDialog::DialogCode::Rejected) {
		return;
	}

	QProxyItem* newItem = dialog.proxyItem();
	if (!newItem) {
		return;
	}

	m_Ui->proxyList->addItem(newItem);
}