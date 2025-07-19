#include "CpWindow.h"

#include <QObject>
#include <QHostAddress>
#include <QMessageBox>

CpWindow::CpWindow() 
	: m_Ui{ new Ui_MainWindow() } {
	m_Ui->setupUi(this);

	m_SocksifierInstance = Socksifier::get();

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
		m_SocksifierInstance->start();

		m_Ui->stopButton->setEnabled(true);
		m_Ui->startButton->setDisabled(true);

		QMessageBox::information(this, QObject::tr("Info"), QObject::tr("Started!"), QMessageBox::StandardButton::Ok);
		});

	QObject::connect(m_Ui->stopButton, &QPushButton::clicked, [this]() -> void {
		if (!m_Started) {
			return;
		}

		m_Started = false;
		m_SocksifierInstance->stop();

		m_Ui->startButton->setEnabled(true);
		m_Ui->stopButton->setDisabled(true);

		QMessageBox::information(this, QObject::tr("Info"), QObject::tr("Stopped!"), QMessageBox::StandardButton::Ok);
		});

	QObject::connect(m_Ui->proxyList, &QListView::customContextMenuRequested, [this](const QPoint& p) -> void {
		QPoint item = m_Ui->proxyList->mapToGlobal(p);

		QMenu submenu{ };
		submenu.addAction("Delete");

		QAction* selectedAction = submenu.exec(item);
		if (selectedAction && selectedAction->text().contains("Delete"))
		{
			QModelIndex modelIndex = m_Ui->proxyList->indexAt(p);

			bool isOk{ };
			std::size_t proxyId = modelIndex.data(Qt::UserRole).toULongLong(&isOk);
			if (!isOk) {
				QMessageBox::critical(this, QObject::tr("Info"), QObject::tr("Can't delete #1!"), QMessageBox::StandardButton::Ok);
				return;
			}

			m_SocksifierInstance->stop();
			if (!m_SocksifierInstance->stopSocks5Proxy(proxyId)) {
				QMessageBox::critical(this, QObject::tr("Info"), QObject::tr("Can't delete #2!"), QMessageBox::StandardButton::Ok);
				return;
			}

			m_Ui->proxyList->takeItem(modelIndex.row());
		}
		});
}

void CpWindow::addProxyItem() {
	auto processImportLineEdit = [this]() -> void {
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

	if (!m_Ui->importLineEdit->text().isEmpty()) {
		processImportLineEdit();
		// (.*):(.*)@(.*):(.*)
	}

	auto validateIPv4Address = [](const QString& ipv4) -> bool {
		return QHostAddress(ipv4).protocol() == QHostAddress::NetworkLayerProtocol::IPv4Protocol;
	};

	if (!validateIPv4Address(m_Ui->ipLineEdit->text())) {
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Enter a valid IPv4 address!"), QMessageBox::StandardButton::Ok);
		return;
	}

	bool isOk{ };
	int port = m_Ui->portLineEdit->text().toInt(&isOk, 10);
	if (!isOk || port <= 0 || port > 65535) {
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Enter a valid port!"), QMessageBox::StandardButton::Ok);
		return;
	}

	static_assert(
		TSupportedProtocols::both == 3 &&
		TSupportedProtocols::udp == 2 &&
		TSupportedProtocols::tcp == 1);

	// obscure code
	// tcp = 1, udp = 2 if we | 'em we'll get TSupportedProtocols::both
	// left shift is needed for the udp to get true value of TSupportedProtocols::udp instead of boolean of a checkbox
	TSupportedProtocols protocols{static_cast<TSupportedProtocols>(
			(static_cast<std::uint8_t>(m_Ui->tcpCheckbox->isChecked()) << 0) | (static_cast<std::uint8_t>(m_Ui->udpCheckbox->isChecked()) << 1)
		)};
	// meaning nothing was selected
	if (static_cast<std::uint8_t>(protocols) == 0) {
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("How are you gonna use proxy without TCP and UDP?"), QMessageBox::StandardButton::Ok);
		return;
	}

	QString endpoint = m_Ui->ipLineEdit->text() + QString(":") + m_Ui->portLineEdit->text();

	std::optional<std::size_t> p = m_SocksifierInstance->addSocks5Proxy(
		endpoint.toStdString(), protocols, true, 
		m_Ui->usernameLineEdit->text().toStdString(),
		m_Ui->passwordLineEdit->text().toStdString());
	if (!p.has_value()) {
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Failed creating proxy object!"), QMessageBox::StandardButton::Ok);
		return;
	}

	QListWidgetItem* item = new QListWidgetItem(endpoint);
	item->setData(Qt::UserRole, QVariant(p.value()));
	m_Ui->proxyList->addItem(item);

	m_SocksifierInstance->associateProcessNameToProxy(L"msedge", p.value());
	QMessageBox::information(this, QObject::tr("Info"), QObject::tr("Added %1!").arg(p.value()), QMessageBox::StandardButton::Ok);
}