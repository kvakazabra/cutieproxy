#include "CpProxyDialog.h"
#include "ui_CpProxyDialog.h"

#include <QMessageBox>

CpProxyDialog::CpProxyDialog(QWidget* parent, QProxyItem* edit)
	: m_Ui{ new Ui::ProxyDialog() }, m_Prototype{ edit } {
	m_Ui->setupUi(this);

	init();
	connectSignals();
}

CpProxyDialog::~CpProxyDialog() {
	delete m_Ui;
	m_Ui = nullptr;
}

QProxyItem* CpProxyDialog::proxyItem() {
	QProxyItem* newItem = new QProxyItem(
		m_Ui->ipLineEdit->text(),
		m_Ui->portLineEdit->text(),
		m_Ui->usernameLineEdit->text(),
		m_Ui->passwordLineEdit->text(),
		m_Ui->tcpCheckbox->isChecked(),
		m_Ui->udpCheckbox->isChecked()
	);

	switch (newItem->error()) {
	case TProxyErrorType::Invalid:
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Go rest"), QMessageBox::StandardButton::Ok);
		delete newItem; newItem = nullptr;
		return { };
	case TProxyErrorType::NoProtocols:
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("How are you gonna use proxy without TCP and UDP?"), QMessageBox::StandardButton::Ok);
		delete newItem; newItem = nullptr;
		return { };
	case TProxyErrorType::InvalidIPv4:
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Enter a valid IPv4 address!"), QMessageBox::StandardButton::Ok);
		delete newItem; newItem = nullptr;
		return { };
	case TProxyErrorType::InvalidPort:
		QMessageBox::critical(this, QObject::tr("Error"), QObject::tr("Enter a valid port!"), QMessageBox::StandardButton::Ok);
		delete newItem; newItem = nullptr;
		return { };
	default:
		break;
	}

	if (m_Prototype && newItem) {
		m_Prototype->copyFiltersTo(newItem);
	}

	return newItem;
}

void CpProxyDialog::init() {
	if (m_Prototype) {
		m_Ui->ipLineEdit->setText(m_Prototype->ipv4());
		m_Ui->portLineEdit->setText(m_Prototype->port());
		m_Ui->usernameLineEdit->setText(m_Prototype->username());
		m_Ui->passwordLineEdit->setText(m_Prototype->password());

		m_Ui->tcpCheckbox->setChecked(m_Prototype->protocols() & TSupportedProtocols::tcp);
		m_Ui->udpCheckbox->setChecked(m_Prototype->protocols() & TSupportedProtocols::udp);

		m_Ui->addProxyButton->setText(QObject::tr("Apply Changes"));
	}
}

void CpProxyDialog::connectSignals() {
	QObject::connect(m_Ui->closeButton, &QPushButton::clicked, this, &QDialog::reject);
	QObject::connect(m_Ui->addProxyButton, &QPushButton::clicked, this, &QDialog::accept);

	QObject::connect(m_Ui->importLineEdit, &QLineEdit::textChanged, [this]() -> void {
		if (m_Ui->importLineEdit->text().isEmpty()) {
			return;
		}

		QRegularExpression regex("(.*):(.*)@(.*):(.*)", QRegularExpression::PatternOption::NoPatternOption);
		QRegularExpressionMatch match = regex.match(m_Ui->importLineEdit->text());

		QStringList captured = match.capturedTexts();
		if (captured.size() != 5) {
			return;
		}

		QLineEdit* const fields[4] = {
			m_Ui->usernameLineEdit,
			m_Ui->passwordLineEdit,
			m_Ui->ipLineEdit,
			m_Ui->portLineEdit
		};

		for (qsizetype i = 0; i < captured.size() && i < 4; i++) {
			fields[i]->setText(captured[i + 1]);
		}
		});
}