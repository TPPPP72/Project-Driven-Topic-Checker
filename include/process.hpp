#pragma once
#include "filesys.hpp"
#include <string>
#include <thread>
#include <vector>
#include <windows.h>

namespace Win32
{
class Process
{
  public:
    struct Result
    {
        int exit_code;
        SafeString output;
        SafeString error;
    };

    Result run_cmd(const SafeString &path, const std::vector<SafeString> &args)
    {
        std::wstring cmd_line{_make_command_line(path, args)};
        return _createprocess(cmd_line);
    }

    Result run_cmd(const Filesys::WorkingDir &dir, const SafeString &path, const std::vector<SafeString> &args)
    {
        std::wstring cmd_line{_make_command_line(path, args)};
        return _createprocess(cmd_line, dir.get_root());
    }

  private:
    SafeString _make_command_line(const SafeString &exe, const std::vector<SafeString> &args)
    {
        std::string result;
        auto add_quote = [](const std::string &str) {
            std::string res = str;
            if (str.find(" ") != std::string::npos && str.find("\\") != std::string::npos &&
                str.find(".") != std::string::npos)
            {
                res = '\"' + str + '\"';
            }
            return res;
        };
        result += add_quote(exe.str());
        for (const auto &item : args)
        {
            result += ' ';
            result += add_quote(item.str());
        }
        return result;
    }

    SafeString _read_pipe(HANDLE pipe)
    {
        char buffer[4096];
        DWORD bytesRead;
        std::string result;

        while (true)
        {
            BOOL success = ReadFile(pipe, buffer, sizeof(buffer), &bytesRead, nullptr);
            if (!success || bytesRead == 0)
                break;

            result.append(std::string(buffer, bytesRead));
        }

        return result;
    }

    Result _createprocess(std::wstring &command_line, std::wstring env = L"")
    {
        SECURITY_ATTRIBUTES sa{};
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;

        HANDLE stdoutRead, stdoutWrite;
        HANDLE stderrRead, stderrWrite;

        CreatePipe(&stdoutRead, &stdoutWrite, &sa, 0);
        CreatePipe(&stderrRead, &stderrWrite, &sa, 0);

        SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(stderrRead, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOW si{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = stdoutWrite;
        si.hStdError = stderrWrite;
        si.hStdInput = NULL;

        PROCESS_INFORMATION pi{};

        BOOL success;
        if (env.empty())
        {
            success =
                CreateProcessW(nullptr, command_line.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi);
        }
        else
        {
            success =
                CreateProcessW(nullptr, command_line.data(), nullptr, nullptr, TRUE, 0, nullptr, env.c_str(), &si, &pi);
        }

        CloseHandle(stdoutWrite);
        CloseHandle(stderrWrite);

        SafeString out, err;
        std::thread tOut([&] { out = _read_pipe(stdoutRead); });
        std::thread tErr([&] { err = _read_pipe(stderrRead); });

        WaitForSingleObject(pi.hProcess, INFINITE);
        tOut.join();
        tErr.join();

        CloseHandle(stdoutRead);
        CloseHandle(stderrRead);

        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return Result{static_cast<int>(exitCode), out, err};
    }
};

} // namespace Win32
