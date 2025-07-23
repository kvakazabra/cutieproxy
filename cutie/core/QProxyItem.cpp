#include "QProxyItem.h"

#include <QHostAddress>
#include <QMessageBox>

QProxyItem::QProxyItem(const QString& ipv4, const QString& port, const QString& username,
	const QString& password, bool tcpChecked, bool udpChecked) :
	m_IPv4{ ipv4 }, m_Port{ port }, m_Username{ username }, m_Password{ password } {

	m_ErrorType = TProxyErrorType::Invalid;

	if (m_IPv4.isEmpty()) {
		m_ErrorType = TProxyErrorType::InvalidIPv4;
		return;
	}

	if (m_Port.isEmpty()) {
		m_ErrorType = TProxyErrorType::InvalidPort;
		return;
	}

	QListWidgetItem::setText(m_IPv4 + "\n" + m_Username);

	static auto validateIPv4Address = [](const QString& ipv4) -> bool {
		return QHostAddress(ipv4).protocol() == QHostAddress::NetworkLayerProtocol::IPv4Protocol;
	};

	if (!validateIPv4Address(m_IPv4)) {
		m_ErrorType = TProxyErrorType::InvalidIPv4;
		return;
	}

	bool isOk{ };
	int iPort = m_Port.toInt(&isOk, 10);
	if (!isOk || iPort <= 0 || iPort > 65535) {
		m_ErrorType = TProxyErrorType::InvalidPort;
		return;
	}

	static_assert(
		TSupportedProtocols::both == 3 &&
		TSupportedProtocols::udp == 2 &&
		TSupportedProtocols::tcp == 1);

	// obscure code
	// tcp = 1, udp = 2 if we | 'em we'll get TSupportedProtocols::both
	// left shift is needed for the udp to get true value of TSupportedProtocols::udp instead of boolean of a checkbox
	m_Protocols = 
		static_cast<TSupportedProtocols>(
			(static_cast<std::uint8_t>(tcpChecked) << 0) | (static_cast<std::uint8_t>(udpChecked) << 1)
		);

	// meaning nothing was selected
	if (static_cast<std::uint8_t>(m_Protocols) == 0) {
		m_ErrorType = TProxyErrorType::NoProtocols;
		return;
	}

	m_ErrorType = TProxyErrorType::None;
}

QString QProxyItem::endpoint() const {
	return QString("%1:%2").arg(m_IPv4).arg(m_Port);
}

const QString& QProxyItem::ipv4() const {
	return m_IPv4;
}

const QString& QProxyItem::port() const {
	return m_Port;
}

const QString& QProxyItem::username() const {
	return m_Username;
}

const QString& QProxyItem::password() const {
	return m_Password;
}

TSupportedProtocols QProxyItem::protocols() const {
	return m_Protocols;
}

TProxyErrorType QProxyItem::error() const {
	return m_ErrorType;
}

void QProxyItem::addFilter(const std::shared_ptr<Filter>& filter) {
	m_Filters.emplace_back(filter);
}

void QProxyItem::copyFiltersFrom(QProxyItem* from) {
	this->m_Filters = from->m_Filters;
}

void QProxyItem::copyFiltersTo(QProxyItem* to) {
	to->m_Filters = this->m_Filters;
}

const std::vector<std::shared_ptr<Filter>>& QProxyItem::filters() {
	return m_Filters;
}