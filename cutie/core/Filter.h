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

class NoneFilter : public iphelper::filter_interface {
public:
    NoneFilter() = default;
    ~NoneFilter() = default;

    NoneFilter(const NoneFilter& other) = default;
    NoneFilter& operator=(const NoneFilter& other) = default;
public:
    virtual bool match(const std::shared_ptr<iphelper::network_process>& process) const override {
        return false;
    }
};

class AllFilter : public iphelper::filter_interface {
public:
    AllFilter() = default;
    ~AllFilter() = default;

    AllFilter(const AllFilter& other) = default;
    AllFilter& operator=(const AllFilter& other) = default;
public:
    virtual bool match(const std::shared_ptr<iphelper::network_process>& process) const override {
        return true;
    }
};

enum class TNameFilterType {
    ProcessName,
    ImagePath,
};

enum class TStringMatchType {
    Exact,
    Contains,
    RegExp,
};

class NameFilter : public iphelper::filter_interface {
public:
    NameFilter(TNameFilterType type, const std::wstring& filter, TStringMatchType stringMatchType = TStringMatchType::Exact, bool caseSensitive = true);
    ~NameFilter() = default;

    NameFilter(const NameFilter& other) = default;
    NameFilter& operator=(const NameFilter& other) = default;
public:
    virtual bool match(const std::shared_ptr<iphelper::network_process>& process) const override;

    auto pattern() const -> const std::wstring&;
    auto isCaseSensitive() const -> bool;
    auto data() const -> const std::wstring&;
private:
    std::wstring m_Data{ };
    TStringMatchType m_StringMatchType{ };
    bool m_IsCaseSensitive{ };
    TNameFilterType m_FilterType{ };
};

class ProcessIdFilter : public iphelper::filter_interface {
public:
    using TProcessId = decltype(iphelper::network_process::id);
public:
    ProcessIdFilter(std::set<TProcessId> filter);
    ~ProcessIdFilter() = default;

    ProcessIdFilter(const ProcessIdFilter& other) = default;
    ProcessIdFilter& operator=(const ProcessIdFilter& other) = default;
public:
    virtual bool match(const std::shared_ptr<iphelper::network_process>& process) const override;

    auto ids() const -> const std::set<TProcessId>&;
private:
    std::set<TProcessId> m_Data{ };
};