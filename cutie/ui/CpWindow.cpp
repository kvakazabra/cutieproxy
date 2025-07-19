#include "CpWindow.h"

#include "../core/QProxyItem.h"

#include <QObject>
#include <QHostAddress>
#include <QMessageBox>

CpWindow::CpWindow() 
	: m_Ui{ new Ui_MainWindow() } {
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
		submenu.addAction("Delete");

		QAction* selectedAction = submenu.exec(item);
		if (selectedAction && selectedAction->text().contains("Delete")) {
			QModelIndex modelIndex = m_Ui->proxyList->indexAt(p);

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
	auto processImportLineEdit = [this]() -> void {
		if (m_Ui->importLineEdit->text().isEmpty()) {
			return;
		}

		QRegularExpression regex("(.*):(.*)@(.*):(.*)", QRegularExpression::PatternOption::NoPatternOption);
		QRegularExpressionMatch match = regex.match(m_Ui->importLineEdit->text());
		
		QStringList captured = match.capturedTexts();
		if (captured.size() != 5) {
			return;
		}

		static QLineEdit* const fields[4] = {
			m_Ui->usernameLineEdit,
			m_Ui->passwordLineEdit,
			m_Ui->ipLineEdit,
			m_Ui->portLineEdit
		};

		for (qsizetype i = 0; i < captured.size() && i < 4; i++) {
			fields[i]->setText(captured[i + 1]);
		}
	};

	processImportLineEdit();

	QProxyItem* item = new QProxyItem(
		m_Ui->ipLineEdit->text(), 
		m_Ui->portLineEdit->text(), 
		m_Ui->usernameLineEdit->text(), 
		m_Ui->passwordLineEdit->text(), 
		m_Ui->tcpCheckbox->isChecked(), 
		m_Ui->udpCheckbox->isChecked()
	);

	switch (item->error()) {
	case TProxyErrorType::NoProtocols:
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("How are you gonna use proxy without TCP and UDP?"), QMessageBox::StandardButton::Ok);
		delete item; item = nullptr;
		return;
	case TProxyErrorType::InvalidIPv4:
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Enter a valid IPv4 address!"), QMessageBox::StandardButton::Ok);
		delete item; item = nullptr;
		return;
	case TProxyErrorType::InvalidPort:
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Enter a valid port!"), QMessageBox::StandardButton::Ok);
		delete item; item = nullptr;
		return;
	default:
		break;
	}

	m_Ui->proxyList->addItem(item);

	/*std::optional<std::size_t> p = m_Socksifier->addSocks5Proxy(
		endpoint.toStdString(), protocols, true, 
		m_Ui->usernameLineEdit->text().toStdString(),
		m_Ui->passwordLineEdit->text().toStdString());
	if (!p.has_value()) {
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Failed creating proxy object!"), QMessageBox::StandardButton::Ok);
		return;
	}*/

	//item->setData(Qt::UserRole, QVariant(p.value()));

	//m_Socksifier->associateProcessNameToProxy(m_Ui->procName->text().toStdWString(), p.value());
	//QMessageBox::information(this, QObject::tr("Info"), QObject::tr("Added %1!").arg(p.value()), QMessageBox::StandardButton::Ok);
}