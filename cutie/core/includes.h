#pragma once
#define NOMINMAX 1

#pragma comment(lib, "ndisapi.lib")
#pragma comment(lib, "Iphlpapi.lib")

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <in6addr.h>
#include <tchar.h>
#include <ws2ipdef.h>
#include <IPHlpApi.h>
#include <Mstcpip.h>
#include <WinDNS.h>
#include <conio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <optional>
#include <functional>
#include <bitset>
#include <variant>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <iostream>
#include <fstream>
#include <stack>
#include <charconv>
#include <unordered_set>
#include <queue>
#include <regex>
#include <syncstream>
#include <chrono>

#include <gsl/gsl>

#define _LIB

#include <ndisapi/include/Common.h>
#include <ndisapi/include/ndisapi.h>

#undef _LIB

#include <netlib/log/log.h>
#include <netlib/tools/generic.h>
#include <netlib/iphlp.h>
#include <netlib/winsys/object.h>
#include <netlib/winsys/event.h>
#include <netlib/winsys/io_completion_port.h>
#include <netlib/net/mac_address.h>
#include <netlib/net/ip_address.h>
#include <netlib/net/ip_subnet.h>
#include <netlib/net/ip_endpoint.h>
#include <netlib/net/ipv6_helper.h>
#include <netlib/pcap/pcap.h>
#include <netlib/iphelper/network_adapter_info.h>
#include <netlib/ndisapi/network_adapter.h>
#include <netlib/ndisapi/intermediate_buffer.h>
#include <netlib/ndisapi/queued_packet_filter.h>
#include <netlib/ndisapi/static_filters.h>
#include <netlib/pcap/pcap.h>
#include <netlib/pcap/pcap_file_storage.h>
#include <netlib/ndisapi/tcp_local_redirect.h>
#include <netlib/proxy/proxy_common.h>
#include <netlib/proxy/socks5_common.h>
#include <netlib/ndisapi/socks5_udp_local_redirect.h>
#include <netlib/winsys/io_completion_port.h>
#include <netlib/proxy/packet_pool.h>
#include <netlib/proxy/tcp_proxy_socket.h>
#include <netlib/proxy/socks5_tcp_proxy_socket.h>
#include <netlib/proxy/tcp_proxy_server.h>
#include <netlib/proxy/socks5_udp_proxy_socket.h>
#include <netlib/proxy/socks5_local_udp_proxy_server.h>
#include <netlib/iphelper/network_adapter_info.h>
#include <netlib/iphelper/process_lookup.h>
#include <netlib/proxy/socks_local_router.h>

using TLogLevel = netlib::log::log_level;

enum class TStatus
{
    stopped,
    connected,
    disconnected,
    error
};

using TSupportedProtocols = proxy::socks_local_router::supported_protocols;

enum class TEventType : uint32_t
{
    connected,
    disconnected,
    address_error,
};

struct TEvent
{
    TEventType type; // event type
    size_t data; // optional data
};

using TLogEntry = std::variant<std::string, TEvent>;
using TLogStorage = std::vector<std::pair<long long, TLogEntry>>;

//class Logger
//{
//
//    static constexpr auto default_log_limit = 100;
//    TLogStorage log_storage_;
//    std::mutex log_storage_lock_;
//    size_t log_limit_{ default_log_limit };
//    HANDLE log_event_{ nullptr };
//
//    /**
//     * @brief Custom stream buffer for logging.
//     */
//    class log_streambuf final : public std::streambuf
//    {
//        logger* log_;
//        std::string buffer_;
//
//    protected:
//        /**
//         * @brief Handles overflow by writing to the logger.
//         * @param c The character to write.
//         * @return The character written.
//         */
//        int overflow(const int c) override
//        {
//            if (c != EOF)
//            {
//                buffer_ += static_cast<char>(c);
//                if (c == '\n')
//                {
//                    log_->log_printer(buffer_.c_str());
//                    buffer_.clear();
//                }
//            }
//            return c;
//        }
//
//    public:
//        /**
//         * @brief Constructs the log_streambuf with a logger pointer.
//         * @param log Pointer to the logger instance.
//         */
//        explicit log_streambuf(logger* log) : log_(log)
//        {
//        }
//    };
//
//    log_streambuf streambuf_;
//    std::ostream log_stream_;
//
//    /**
//     * @brief Private constructor to enforce singleton pattern.
//     */
//    logger() : streambuf_(this), log_stream_(&streambuf_)
//    {
//    }
//
//public:
//    /**
//     * @brief Gets the singleton instance of the logger.
//     * @return Pointer to the logger instance.
//     */
//    static logger* get_instance()
//    {
//        static logger inst; // NOLINT(clang-diagnostic-exit-time-destructors)
//        return &inst;
//    }
//
//    /**
//     * @brief Logs a message.
//     * @param log The message to log.
//     */
//    void log_printer(const char* log)
//    {
//        using namespace std::chrono;
//        const auto ms = duration_cast<milliseconds>(
//            system_clock::now().time_since_epoch()
//        );
//
//        std::lock_guard lock(log_storage_lock_);
//
//        log_storage_.emplace_back(ms.count(), log);
//
//        if (log_event_ && log_storage_.size() > log_limit_)
//            ::SetEvent(log_event_);
//    }
//
//    /**
//     * @brief Logs an event.
//     * @param log_event The event to log.
//     */
//    void log_event(const TEvent log_event)
//    {
//        using namespace std::chrono;
//
//        switch (log_event.type)
//        {
//        case TEventType::address_error:
//        case TEventType::connected:
//        case TEventType::disconnected:
//        {
//            const auto ms = duration_cast<milliseconds>(
//                system_clock::now().time_since_epoch()
//            );
//
//            std::lock_guard lock(log_storage_lock_);
//
//            log_storage_.emplace_back(ms.count(), log_event);
//        }
//        break;
//        }
//
//        if (log_event_ && log_storage_.size() > log_limit_)
//            ::SetEvent(log_event_);
//    }
//
//    /**
//     * @brief Reads the log storage.
//     * @return Optional containing the log storage if not empty.
//     */
//    std::optional<TLogStorage> read_log()
//    {
//        using namespace std::chrono;
//
//        std::lock_guard lock(log_storage_lock_);
//
//        return log_storage_.empty() ? std::nullopt : std::make_optional(std::move(log_storage_));
//    }
//
//    /**
//     * @brief Gets the size of the log storage.
//     * @return The size of the log storage.
//     */
//    size_t size()
//    {
//        std::lock_guard lock(log_storage_lock_);
//        return log_storage_.size();
//    }
//
//    /**
//     * @brief Sets the log limit.
//     * @param log_limit The new log limit.
//     */
//    void set_log_limit(const uint32_t log_limit)
//    {
//        log_limit_ = log_limit;
//    }
//
//    /**
//     * @brief Gets the log limit.
//     * @return The log limit.
//     */
//    [[nodiscard]] uint32_t get_log_limit() const
//    {
//        return static_cast<uint32_t>(log_limit_);
//    }
//
//    /**
//     * @brief Sets the log event handle.
//     * @param log_event The log event handle.
//     */
//    void set_log_event(HANDLE log_event)
//    {
//        log_event_ = log_event;
//    }
//
//    /**
//     * @brief Gets the log stream.
//     * @return Reference to the log stream.
//     */
//    std::ostream& get_log_stream()
//    {
//        return log_stream_;
//    }
//};