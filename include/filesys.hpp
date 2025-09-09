#pragma once

#include "sha256.h"
#include "win_safe_fstream.hpp"
#include "win_safe_string.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <vector>

namespace Filesys
{
template <typename T> void exists(const T &path)
{
    if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path))
    {
        return;
    }
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
    {
        return;
    }
    throw std::runtime_error(path + " not found!");
}

class WorkingDir
{
  public:
    WorkingDir()
    {
        data = ".\\";
    }
    SafeString get_dir(const SafeString &dir) const
    {
        if (dir.back() != '\\')
            return data + '\\' + dir;
        else
            return data + dir;
    }
    SafeString get_root() const
    {
        return data;
    }
    void getline(std::istream &is)
    {
        data.getline(is);
        std::string s{data};
        if (s.front() == '\"')
            s.erase(s.begin());
        if (s.back() == '\"')
            s.pop_back();
        data = s;
    }
    friend std::istream &operator>>(std::istream &is, WorkingDir &working_dir)
    {
        is >> working_dir.data;
        return is;
    }

  private:
    SafeString data;
};

inline std::string getSHA256(const std::string &path)
{
    std::ifstream rd(path, std::ios::binary);
    if (!rd.is_open())
    {
        throw std::runtime_error("Failed to open file: " + path);
    }

    rd.seekg(0, std::ios::end);
    std::streamsize size = rd.tellg();
    rd.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!rd.read(buffer.data(), size))
    {
        throw std::runtime_error("Failed to read file: " + path);
    }

    SHA256 sha256;
    return sha256(buffer.data(), buffer.size());
}

inline std::string getSHA256(const SafeString &path)
{
    SafeIfstream rd(path, std::ios::binary);
    if (!rd.is_open())
    {
        throw std::runtime_error(std::format("Failed to open file: {}", path.str()));
    }

    rd.seekg(0, std::ios::end);
    std::streamsize size = rd.tellg();
    rd.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!rd.read(buffer.data(), size))
    {
        throw std::runtime_error(std::format("Failed to read file: {}", path.str()));
    }

    SHA256 sha256;
    return sha256(buffer.data(), buffer.size());
}

template <typename Tp, typename Up> bool compare(const Tp &path1, const Up &path2)
{
    return getSHA256(path1) == getSHA256(path2);
}

} // namespace Filesys
