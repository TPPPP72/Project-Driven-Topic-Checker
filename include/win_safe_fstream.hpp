#pragma once

#include "win_safe_string.hpp"
#include <fstream>

class SafeIfstream : public std::ifstream
{
  public:
    SafeIfstream() = default;

    SafeIfstream(const std::string &, std::ios_base::openmode mode = std::ios_base::in) = delete;

    SafeIfstream(const char *, std::ios_base::openmode mode = std::ios_base::in) = delete;

    SafeIfstream(const SafeString &s, std::ios_base::openmode mode = std::ios_base::in)
        : std::ifstream(std::filesystem::path(s), mode)
    {
    }
};

class SafeOfstream : public std::ofstream
{
  public:
    SafeOfstream() = default;

    SafeOfstream(const std::string &, std::ios_base::openmode mode = std::ios_base::out) = delete;

    SafeOfstream(const char *, std::ios_base::openmode mode = std::ios_base::out) = delete;

    SafeOfstream(const SafeString &s, std::ios_base::openmode mode = std::ios_base::out)
        : std::ofstream(std::filesystem::path(s), mode)
    {
    }
};