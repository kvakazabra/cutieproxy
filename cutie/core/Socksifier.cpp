#include "Socksifier.h"

Socksifier::Socksifier(const TLogLevel logLevel) :
    m_LogLevel{ logLevel } {
    WSADATA wsaData{ };
    if (WORD versionRequested = MAKEWORD(2, 2); ::WSAStartup(versionRequested, &wsaData) != 0) {
        printLog(TLogLevel::info, "WSAStartup failed with error\n");
    }

    printLog(TLogLevel::info, "Creating SOCKS5 Local Router instance...");

    m_Proxy = std::make_unique<proxy::socks_local_router>(logLevel);// , logger::get_instance()->get_log_stream());

    if (!m_Proxy) {
        printLog(TLogLevel::info, "[ERROR]: Failed to create the SOCKS5 Local Router instance!");
        throw std::runtime_error("[ERROR]: Failed to create the SOCKS5 Local Router instance!");
    }

    printLog(TLogLevel::info, "SOCKS5 Local Router instance successfully created.");
}

Socksifier::~Socksifier() {
    WSACleanup();
}

bool Socksifier::start() const {
    std::lock_guard lock(m_Lock);

    if (!m_Proxy->start()) {
        printLog(TLogLevel::info, "[ERROR]: Failed to start the SOCKS5 Local Router instance!");
        return false;
    }

    printLog(TLogLevel::info, "SOCKS5 Local Router instance started successfully.");
    return true;
}

bool Socksifier::stop() const {
    std::lock_guard lock(m_Lock);

    if (!m_Proxy) {
        printLog(TLogLevel::info,
            "[ERROR]: Failed to stop the SOCKS5 Local Router instance. Instance does not exist.");
        return false;
    }

    if (m_Proxy->stop()) {
        printLog(TLogLevel::info, "[ERROR]: Failed to stop the SOCKS5 Local Router instance.");
        return false;
    }

    printLog(TLogLevel::info, "SOCKS5 Local Router instance stopped successfully.");

    return true;
}

std::optional<std::size_t> Socksifier::addSocks5Proxy(
    const std::string& endpoint,
    const TSupportedProtocols protocols,
    const bool start,
    const std::string& login,
    const std::string& password) const {
    std::optional<std::pair<std::string, std::string>> cred{ std::nullopt };

    if (!login.empty()) {
        cred = std::make_pair(login, password);
    }

    return m_Proxy->add_socks5_proxy(endpoint, protocols, cred, start);
}

bool Socksifier::stopSocks5Proxy(std::size_t proxyId) const {
    return m_Proxy->stop_socks5_proxy(proxyId);
}

bool Socksifier::addFilterToProxy(std::size_t proxy_id, const std::shared_ptr<Filter>& filter) const {
    return m_Proxy->add_filter_to_proxy(proxy_id, filter);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void Socksifier::setLogLimit(const uint32_t log_limit) {
    //logger::get_instance()->set_log_limit(log_limit);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
uint32_t Socksifier::getLogLimit() {
    return { };
    //return logger::get_instance()->get_log_limit();
}

void Socksifier::setLogEvent(HANDLE log_event) {
    //logger::get_instance()->set_log_event(log_event);
}

TLogStorage Socksifier::readLog() {
    return { };
    //return logger::get_instance()->read_log().value_or(TLogStorage{});
}

void Socksifier::logPrinter(const char* log) {
    //logger::get_instance()->log_printer(log);
}

void Socksifier::logEvent(const TEvent log) {
    //logger::get_instance()->log_event(log);
}

void Socksifier::printLog(const TLogLevel level, const std::string& message) const {
    if (level < m_LogLevel) {
        logPrinter(message.c_str());
    }
}