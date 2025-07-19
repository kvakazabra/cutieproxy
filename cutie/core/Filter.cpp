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

Filter::Filter(TFilterType type)
    : m_FilterType{ type } {
}
Filter::Filter(TFilterType type, const std::wstring& filter, TStringMatchType stringMatchType, bool caseSensitive)
    : m_FilterType{ type }, m_Filter{ filter }, m_StringMatchType{ stringMatchType }, m_IsCaseSensitive{ caseSensitive } {
}
Filter::Filter(TFilterType type, std::set<TProcessId> filter)
    : m_FilterType{ type }, m_Filter{ filter } {
}

bool Filter::match(const std::shared_ptr<iphelper::network_process>& process) const {
    try {
        switch (m_FilterType) {
        case TFilterType::None: return false;
        case TFilterType::All: return true;
        case TFilterType::ProcessId:
            return ids().count(process->id) != 0;
            //return std::get<std::uint32_t>(m_Filter) == process->processId();
        case TFilterType::ImagePath:
        case TFilterType::ProcessName: {
            std::wstring src =
                m_FilterType == TFilterType::ImagePath ? process->path_name : process->name;
            std::wstring pattern =
                std::get<std::wstring>(m_Filter);

            if (!m_IsCaseSensitive) {
                tolower(src);
                if (m_StringMatchType != TStringMatchType::RegExp) {
                    tolower(pattern);
                }
            }

            return matchString(src, pattern, m_StringMatchType);
        }
        }
    }
    catch (const std::bad_variant_access& ex) {
        ex;
        return false;
    }

    return false;
}

const std::wstring& Filter::pattern() const {
    static const std::wstring emptyString{ };
    try {
        return std::get<std::wstring>(m_Filter);
    }
    catch (...) {
        return emptyString;
    }
}

bool Filter::isCaseSensitive() const {
    return m_IsCaseSensitive;
}

const std::set<Filter::TProcessId>& Filter::ids() const {
    static const std::set<TProcessId> emptySet{ };
    try {
        return std::get<std::set<TProcessId>>(m_Filter);
    }
    catch (...) {
        return emptySet;
    }
}

const std::variant<std::set<Filter::TProcessId>, std::wstring>& Filter::data() const {
    return m_Filter;
}