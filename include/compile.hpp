#pragma once

#include "config.h"
#include "filesys.hpp"
#include "process.hpp"
#include <filesystem>
#include <iostream>
#include <vector>
#include <windows.h>

class Compiler
{
  public:
    Compiler(const Project &p) : _proj(p)
    {
    }
    std::vector<Win32::Process::ProcessResult> run(const Filesys::WorkingDir &dir)
    {
        std::unordered_map<SafeString, std::vector<SafeString>> map = {{"Compiler", {_proj.Compiler}},
                                                                       {"FileList", _proj.Filelist},
                                                                       {"Flag", {_proj.Flag}},
                                                                       {"checker", {"checker.exe"_ss}}};

        std::vector<Win32::Process::ProcessResult> result;
        for (auto &item : _proj.CClist)
        {
            auto res = substitute_placeholders_with_args(item, map);
            std::vector<SafeString> args;
            for (auto &item : res.second)
            {
                std::string str{item};
                for (auto it = str.begin(); it != str.end(); ++it)
                {
                    if (*it != ' ')
                    {
                        break;
                    }
                    it = str.erase(it);
                }

                for (auto it = str.end() - 1; it >= str.begin(); --it)
                {
                    if (*it != ' ')
                    {
                        break;
                    }
                    it = str.erase(it);
                }
                if (!str.empty())
                {
                    args.emplace_back(str);
                }
            }
            std::cout << "编译命令：" << Win32::Process::make_command_line(_proj.Compiler, args) << std::endl;
            auto cmd_res = _process.run_cmd_env(dir, _proj.Compiler, args);
            result.emplace_back(cmd_res);
        }
        return result;
    }

  private:
    Win32::Process _process;
    Project _proj;
};

inline SafeString get_compiler()
{
    auto exists_in_path = [](const SafeString &exe) {
        wchar_t buffer[MAX_PATH];
        std::wstring s{exe.wstr()};
        return SearchPathW(nullptr, s.c_str(), L".exe", MAX_PATH, buffer, nullptr) > 0;
    };

    std::vector<SafeString> candidates;

    candidates.insert(candidates.end(), {"g++", "clang++"});
    for (const auto &exe : candidates)
    {
        if (exists_in_path(exe))
        {
            return exe;
        }
    }

    // std::vector<SafeString> common_paths;
    // common_paths.insert(common_paths.end(), {"C:\\msys64\\ucrt64\\bin\\g++.exe",
    //                                          "C:\\Program Files (x86)\\Dev-Cpp\\MinGW64\\bin\\g++.exe"});

    // for (const auto &path : common_paths)
    // {
    //     if (std::filesystem::exists(path))
    //     {
    //         return path;
    //     }
    // }

    throw std::runtime_error("Compiler not found, please set config.json");
}