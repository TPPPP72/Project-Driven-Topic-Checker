#pragma once

#include "filesys.hpp"
#include "json.hpp"
#include "win_safe_fstream.hpp"
#include "win_safe_string.hpp"

struct Task
{
    int nums;
    bool has_input;
};

struct Project
{
    SafeString Compiler;
    SafeString Flag;
    std::vector<SafeString> CClist;
    std::vector<SafeString> Filelist;
    std::map<SafeString, Task> tasks;
    bool result = false;
};

void to_json(nlohmann::json &j, const Task &t);

void from_json(const nlohmann::json &j, Task &t);

void to_json(nlohmann::json &j, const Project &p);

void from_json(const nlohmann::json &j, Project &p);

template <typename K, typename V>
std::pair<SafeString, std::vector<SafeString>> substitute_placeholders_with_args(
    const SafeString &input, const std::unordered_map<K, V> &replacements)
{
    SafeString compiler;
    std::vector<SafeString> args;

    size_t pos = 0;
    while (pos < input.size())
    {
        size_t start = input.find('$', pos);
        if (start == SafeString::npos)
        {
            SafeString tail = input.substr(pos);
            if (!tail.str().empty())
                args.push_back(tail);
            break;
        }

        size_t end = input.find('$', start + 1);
        if (end == SafeString::npos)
        {
            SafeString tail = input.substr(pos);
            if (!tail.str().empty())
                args.push_back(tail);
            break;
        }

        SafeString prefix = input.substr(pos, start - pos);
        if (!prefix.str().empty())
            args.push_back(prefix);

        SafeString key = input.substr(start + 1, end - start - 1);
        K lookup_key = K(key.str());

        auto it = replacements.find(lookup_key);
        if (it != replacements.end())
        {
            if (key == "Compiler")
            {
                if constexpr (std::is_same_v<V, std::vector<SafeString>>)
                    for (const auto &item : it->second)
                        compiler += item;
                else
                    compiler += it->second;
            }
            else
            {
                if constexpr (std::is_same_v<V, std::vector<SafeString>>)
                    for (const auto &item : it->second)
                        args.push_back(item);
                else
                    args.push_back(it->second);
            }
        }
        else
        {
            args.push_back("$" + key + "$");
        }

        pos = end + 1;
    }

    return {compiler, args};
}

template <typename T> class Config
{
  public:
    Config(const Filesys::WorkingDir &dir, const T &path)
    {
        _path = path;
        _dir = dir;
    }
    void init()
    {
        Filesys::exists(_dir.get_dir(_path));
        SafeIfstream rd(_dir.get_dir("config.json"));
        nlohmann::json j;
        rd >> j;
        _proj = j.get<Project>();
    }
    Project getproject()
    {
        return _proj;
    }

  private:
    Filesys::WorkingDir _dir;
    T _path;
    Project _proj;
};

template <typename Path>
Config(const Filesys::WorkingDir&, const Path&) -> Config<SafeString>;