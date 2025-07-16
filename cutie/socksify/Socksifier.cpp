#include "Socksifier.h"

Socksifier::Socksifier(const TLogLevel log_level) :
    m_LogLevel{ log_level }
{
    using namespace std::string_literals;

    WSADATA wsa_data;

    if (constexpr auto version_requested = MAKEWORD(2, 2); ::WSAStartup(version_requested, &wsa_data) != 0)
    {
        printLog(TLogLevel::info, "WSAStartup failed with error\n");
    }

    printLog(TLogLevel::info, "Creating SOCKS5 Local Router instance..."s);

    auto logLevel = netlib::log::log_level::all;

    switch (m_LogLevel)
    {
    case TLogLevel::none:
        logLevel = netlib::log::log_level::error;
        break;
    case TLogLevel::info:
        logLevel = netlib::log::log_level::info;
        break;
    case TLogLevel::deb:
        logLevel = netlib::log::log_level::debug;
        break;
    case TLogLevel::all:
        logLevel = netlib::log::log_level::all;
        break;
    }
    
    m_Proxy = std::make_unique<proxy::socks_local_router>(logLevel);// , logger::get_instance()->get_log_stream());

    if (!m_Proxy)
    {
        printLog(TLogLevel::info, "[ERROR]: Failed to create the SOCKS5 Local Router instance!"s);
        throw std::runtime_error("[ERROR]: Failed to create the SOCKS5 Local Router instance!");
    }

    printLog(TLogLevel::info, "SOCKS5 Local Router instance successfully created."s);
}

Socksifier::~Socksifier()
{
    WSACleanup();
}

Socksifier* Socksifier::get(const TLogLevel log_level)
{
    static Socksifier inst(log_level); // NOLINT(clang-diagnostic-exit-time-destructors)
    return &inst;
}

bool Socksifier::start() const
{
    using namespace std::string_literals;
    std::lock_guard lock(m_Lock);

    if (!m_Proxy->start())
    {
        printLog(TLogLevel::info, "[ERROR]: Failed to start the SOCKS5 Local Router instance!"s);
        return false;
    }

    printLog(TLogLevel::info, "SOCKS5 Local Router instance started successfully."s);
    return true;
}

bool Socksifier::stop() const
{
    using namespace std::string_literals;
    std::lock_guard lock(m_Lock);

    if (!m_Proxy)
    {
        printLog(TLogLevel::info,
            "[ERROR]: Failed to stop the SOCKS5 Local Router instance. Instance does not exist."s);
        return false;
    }

    if (m_Proxy->stop())
    {
        printLog(TLogLevel::info, "[ERROR]: Failed to stop the SOCKS5 Local Router instance."s);
        return false;
    }

    printLog(TLogLevel::info, "SOCKS5 Local Router instance stopped successfully."s);

    return true;
}

LONG_PTR Socksifier::addSocks5Proxy(
    const std::string& endpoint,
    const TSupportedProtocols protocol,
    const bool start,
    const std::string& login,
    const std::string& password) const
{
    using namespace std::string_literals;
    std::optional<std::pair<std::string, std::string>> cred{ std::nullopt };

    if (login != ""s)
    {
        cred = std::make_pair(login, password);
    }

    proxy::socks_local_router::supported_protocols protocols = proxy::socks_local_router::supported_protocols::both;
    switch (protocol)
    {
    case TSupportedProtocols::tcp:
        protocols = proxy::socks_local_router::supported_protocols::tcp;
        break;
    case TSupportedProtocols::udp:
        protocols = proxy::socks_local_router::supported_protocols::udp;
        break;
    default:
        break;
    }
    if (const auto result = m_Proxy->add_socks5_proxy(endpoint, protocols, cred, start); result)
    {
        return static_cast<LONG_PTR>(result.value());
    }

    return -1;
}

bool Socksifier::associateProcessNameToProxy(const std::wstring& process_name,
    const LONG_PTR proxy_id) const
{
    return m_Proxy->associate_process_name_to_proxy(process_name, static_cast<size_t>(proxy_id));
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void Socksifier::setLogLimit(const uint32_t log_limit)
{
    //logger::get_instance()->set_log_limit(log_limit);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
uint32_t Socksifier::getLogLimit()
{
    return { };
    //return logger::get_instance()->get_log_limit();
}

void Socksifier::setLogEvent(HANDLE log_event)
{
    //logger::get_instance()->set_log_event(log_event);
}

TLogStorage Socksifier::readLog()
{
    return { };
    //return logger::get_instance()->read_log().value_or(TLogStorage{});
}

void Socksifier::logPrinter(const char* log)
{
    //logger::get_instance()->log_printer(log);
}

void Socksifier::logEvent(const TEvent log)
{
    //logger::get_instance()->log_event(log);
}

void Socksifier::printLog(const TLogLevel level, const std::string& message) const
{
    if (level < m_LogLevel)
    {
        logPrinter(message.c_str());
    }
}