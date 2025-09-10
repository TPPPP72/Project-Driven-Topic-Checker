#pragma once
#include "filesys.hpp"
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>

namespace Win32
{
class Process
{
  public:
    struct ProcessResult
    {
        int exit_code;
        SafeString output;
        SafeString error;
    };

    ProcessResult run_cmd(const SafeString &path, const std::vector<SafeString> &args)
    {
        SECURITY_ATTRIBUTES sa{};
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;

        HANDLE stdoutRead = nullptr, stdoutWrite = nullptr;
        HANDLE stderrRead = nullptr, stderrWrite = nullptr;

        if (!CreatePipe(&stdoutRead, &stdoutWrite, &sa, 0))
            throw std::runtime_error("Failed to create stdout pipe");
        if (!CreatePipe(&stderrRead, &stderrWrite, &sa, 0))
            throw std::runtime_error("Failed to create stderr pipe");

        SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(stderrRead, HANDLE_FLAG_INHERIT, 0);

        PROCESS_INFORMATION pi{};
        BOOL success = FALSE;

        STARTUPINFOW si{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = stdoutWrite;
        si.hStdError = stderrWrite;
        si.hStdInput = NULL;

        std::wstring s{make_command_line(path, args).wstr()};

        success = CreateProcessW(nullptr, s.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi);

        CloseHandle(stdoutWrite);
        CloseHandle(stderrWrite);

        if (!success)
            throw std::runtime_error("CreateProcess failed");

        SafeString out = read_pipe(stdoutRead);
        SafeString err = read_pipe(stderrRead);

        CloseHandle(stdoutRead);
        CloseHandle(stderrRead);

        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return ProcessResult{static_cast<int>(exitCode), out, err};
    }

    ProcessResult run_cmd_env(const Filesys::WorkingDir &dir, const SafeString &path,
                              const std::vector<SafeString> &args)
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

        std::wstring s{make_command_line(path, args).wstr()};

        std::wstring env{SafeString::utf8_to_wstring(dir.get_root().str())};

        BOOL success = CreateProcessW(nullptr, s.data(), nullptr, nullptr, TRUE, 0, nullptr, env.c_str(), &si, &pi);

        CloseHandle(stdoutWrite);
        CloseHandle(stderrWrite);

        SafeString out, err;
        std::thread tOut([&] { out = read_pipe(stdoutRead); });
        std::thread tErr([&] { err = read_pipe(stderrRead); });

        WaitForSingleObject(pi.hProcess, INFINITE);
        tOut.join();
        tErr.join();

        CloseHandle(stdoutRead);
        CloseHandle(stderrRead);

        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return ProcessResult{static_cast<int>(exitCode), out, err};
    }

    static SafeString make_command_line(const SafeString &exe, const std::vector<SafeString> &args)
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

  private:
    SafeString read_pipe(HANDLE pipe)
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
};

} // namespace Win32
