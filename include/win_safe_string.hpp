#pragma once

#include "json.hpp"
#include <string>
#include <windows.h>

class SafeString
{
  public:
    SafeString() = default;

    SafeString(const std::wstring &str)
    {
        _data = wstring_to_utf8(str);
    }

    SafeString(const std::string &str)
    {
        _data = str;
    }

    SafeString(const char *s) : _data(s)
    {
    }
    SafeString(char c) : _data(1, c)
    {
    }

    auto back() const
    {
        return _data.back();
    }

    auto front() const
    {
        return _data.front();
    }

    auto size() const
    {
        return _data.size();
    }

    operator const char *() const
    {
        return _data.c_str();
    }

    const std::string &str() const
    {
        return _data;
    }

    std::wstring wstr() const
    {
        return utf8_to_wstring(_data);
    }

    auto empty() const
    {
        return _data.empty();
    }

    auto length() const
    {
        return _data.length();
    }

    std::filesystem::path path() const
    {
        return std::filesystem::path(utf8_to_wstring(_data));
    }

    SafeString &operator+=(const SafeString &rhs)
    {
        _data += rhs._data;
        return *this;
    }

    friend SafeString operator+(const SafeString &lhs, const SafeString &rhs)
    {
        SafeString result(lhs);
        result += rhs;
        return result;
    }

    SafeString substr(size_t pos = 0, size_t count = std::string::npos) const
    {
        SafeString result;
        result._data = _data.substr(pos, count);
        return result;
    }

    size_t find(const SafeString &needle, size_t pos = 0) const
    {
        return _data.find(needle._data, pos);
    }

    size_t rfind(const SafeString &needle, size_t pos = 0) const
    {
        return _data.rfind(needle._data, pos);
    }

    bool operator==(const SafeString &other) const
    {
        return _data == other._data;
    }

    friend bool operator==(const char *lhs, const SafeString &rhs)
    {
        return lhs == rhs;
    }

    bool operator<(const SafeString &rhs) const
    {
        return _data < rhs._data;
    }

    SafeString operator+(char c) const
    {
        return SafeString(_data + c);
    }

    SafeString operator+(const char *s) const
    {
        return SafeString(_data + s);
    }

    SafeString &getline(std::istream &is, char delim = '\n')
    {
        std::getline(is, _data, delim);
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const SafeString &s)
    {
        return os << s.str();
    }

    friend std::istream &operator>>(std::istream &is, SafeString &s)
    {
        std::string temp;
        is >> temp;
        s = SafeString(temp);
        return is;
    }

    friend void to_json(nlohmann::json &j, const SafeString &s)
    {
        j = s._data;
    }

    friend void from_json(const nlohmann::json &j, SafeString &s)
    {
        s = SafeString(j.get<std::string>());
    }

    operator std::filesystem::path() const
    {
        return std::filesystem::path(reinterpret_cast<const char8_t *>(_data.data()),
                                     reinterpret_cast<const char8_t *>(_data.data() + _data.size()));
    }

    const static auto npos = std::string::npos;

    static std::wstring utf8_to_wstring(const std::string &str)
    {
        if (str.empty())
            return L"";
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
        return wstr;
    }

    static std::string wstring_to_utf8(const std::wstring &wstr)
    {
        if (wstr.empty())
            return "";
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string str(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
        return str;
    }

  private:
    std::string _data;
};

inline SafeString operator""_ss(const char *str, std::size_t len)
{
    return SafeString(std::string(str, len));
}

inline SafeString operator""_ss(const wchar_t *str, std::size_t len)
{
    return SafeString(std::wstring(str, len));
}

namespace std
{
template <> struct hash<SafeString>
{
    std::size_t operator()(const SafeString &s) const noexcept
    {
        return std::hash<std::string>()(s.str()); // 使用内部 UTF-8 string 哈希
    }
};
} // namespace std