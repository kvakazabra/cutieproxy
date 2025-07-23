#include "Filter.h"

static void tolower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) -> char {
        return std::tolower(c);
        });
}

static void tolower(std::wstring& str) { // this is bullshit code, wont work with other languages/cp's
    std::transform(str.begin(), str.end(), str.begin(), [](wchar_t c) -> char {
        return std::tolower(c);
    });
}

static bool matchString(const std::wstring& src, const std::wstring& p, TStringMatchType stringMatchType) {
    switch (stringMatchType) {
    case TStringMatchType::Exact:
        return src == p;
    case TStringMatchType::Contains:
        return src.find(p) != std::string::npos;
    case TStringMatchType::RegExp: {
        return std::regex_match(src, std::wregex(p));
    }
    }

    return false;
}

NameFilter::NameFilter(TNameFilterType type, const std::wstring& filter, TStringMatchType stringMatchType, bool caseSensitive)
    : m_FilterType{ type }, m_Data{ filter }, m_StringMatchType{ stringMatchType }, m_IsCaseSensitive{ caseSensitive } {
}

bool NameFilter::match(const std::shared_ptr<iphelper::network_process>& process) const {
    switch (m_FilterType) {
    case TNameFilterType::ImagePath:
    case TNameFilterType::ProcessName: {
        std::wstring src =
            m_FilterType == TNameFilterType::ImagePath ? process->path_name : process->name;
        std::wstring pattern = m_Data;

        if (!m_IsCaseSensitive) {
            tolower(src);
            if (m_StringMatchType != TStringMatchType::RegExp) {
                tolower(pattern);
            }
        }

        return matchString(src, pattern, m_StringMatchType);
    }
    default: return false;
    }
}

const std::wstring& NameFilter::pattern() const {
    return m_Data;
}

bool NameFilter::isCaseSensitive() const {
    return m_IsCaseSensitive;
}

const std::wstring& NameFilter::data() const {
    return m_Data;
}

ProcessIdFilter::ProcessIdFilter(std::set<TProcessId> filter)
    : m_Data{ filter } {
}

const std::set<ProcessIdFilter::TProcessId>& ProcessIdFilter::ids() const {
    return m_Data;
}

bool ProcessIdFilter::match(const std::shared_ptr<iphelper::network_process>& process) const {
    return m_Data.count(process->id) != 0;
}