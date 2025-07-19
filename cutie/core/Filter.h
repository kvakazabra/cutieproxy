#pragma once
#include <string>
#include <cstdint>
#include <unordered_map>
#include <algorithm>
#include <regex>
#include <utility>
#include <variant>
#include <memory>

#include "includes.h"

enum class TFilterType {
    None,
    All,
    ProcessName,
    ImagePath,
    ProcessId,
};

enum class TStringMatchType {
    Exact,
    Contains,
    RegExp,
};

class Filter : public iphelper::filter_interface {
    using TProcessId = decltype(iphelper::network_process::id);
public:
    Filter(TFilterType type);
    Filter(TFilterType type, const std::wstring& filter, TStringMatchType stringMatchType = TStringMatchType::Exact, bool caseSensitive = true);
    Filter(TFilterType type, std::set<TProcessId> filter);

    ~Filter() = default;

    Filter(const Filter& other) = default;
    Filter& operator=(const Filter& other) = default;
public:
    virtual bool match(const std::shared_ptr<iphelper::network_process>& process) const override;

    auto pattern() const -> const std::wstring&;
    auto isCaseSensitive() const -> bool;
    auto ids() const -> const std::set<TProcessId>&;
    auto data() const -> const std::variant<std::set<TProcessId>, std::wstring>&;
private:
    std::variant<std::set<TProcessId>, std::wstring> m_Filter{ };
    TStringMatchType m_StringMatchType{ };
    bool m_IsCaseSensitive{ };
    TFilterType m_FilterType{ };
};