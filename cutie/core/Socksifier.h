#pragma once
#include "includes.h"
#include "Filter.h"

namespace proxy {
    class socks_local_router;
}

class Socksifier {
public:
    explicit Socksifier(TLogLevel log_level);
    ~Socksifier();

    Socksifier(const Socksifier& other) = delete;
    Socksifier(Socksifier&& other) = delete;
    Socksifier& operator=(const Socksifier& other) = delete;
    Socksifier& operator=(Socksifier&& other) = delete;

    //static Socksifier* get(TLogLevel log_level = TLogLevel::all);

    bool start() const;
    bool stop() const;
    std::optional<std::size_t> addSocks5Proxy(
        const std::string& endpoint,
        TSupportedProtocols protocol,
        bool start = false,
        const std::string& login = "",
        const std::string& password = ""
    ) const;
    bool stopSocks5Proxy(std::size_t proxyId) const;
    bool addFilterToProxy(std::size_t proxy_id, const std::shared_ptr<Filter>& filter) const;
    void setLogLimit(uint32_t log_limit);
    uint32_t getLogLimit();
    void setLogEvent(HANDLE log_event);
    TLogStorage readLog();

private:
    static void logPrinter(const char* log);
    static void logEvent(TEvent log);
    void printLog(TLogLevel level, const std::string& message) const;

    TLogLevel m_LogLevel{ TLogLevel::error };
    std::string m_Address;
    std::unique_ptr<proxy::socks_local_router> m_Proxy;
    mutable std::mutex m_Lock;
};