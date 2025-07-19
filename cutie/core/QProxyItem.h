#pragma once
#include <QListWidgetItem>

#include "includes.h"
#include "Filter.h"

enum class TProxyErrorType {
	None,
	Invalid,
	InvalidIPv4,
	InvalidPort,
	InvalidCredentials,
	NoProtocols,
};

class QProxyItem : public QListWidgetItem {
public:
	QProxyItem(
		const QString& ipv4, 
		const QString& port, 
		const QString& username, 
		const QString& password,
		bool tcpChecked, bool udpChecked);

	QString endpoint() const;
	const QString& ipv4() const;
	const QString& port() const;
	const QString& username() const;
	const QString& password() const;
	TSupportedProtocols protocols() const;

	TProxyErrorType error() const;

	void addFilter(const std::shared_ptr<Filter>& filter);
	const std::vector<std::shared_ptr<Filter>>& filters();
private:
	QString m_IPv4{ }, m_Port{ };
	QString m_Username{ }, m_Password{ };
	TSupportedProtocols m_Protocols{ };

	TProxyErrorType m_ErrorType{ };
	std::vector<std::shared_ptr<Filter>> m_Filters{ };
};