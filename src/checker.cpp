#include "../include/checker.h"
#include "../include/filesys.hpp"
#include "../include/miniz.h"
#include "../include/process.hpp"
#include <iostream>

void Checker::single(int id, int num)
{
    std::cout << "测试Task" << id << ":" << std::endl;
    Win32::Process process;
    process.run_cmd_env(_dir, _dir.get_dir("checker.exe"), {std::format("Task{}_{}", id, num)});
    std::vector<std::pair<SafeString, SafeString>> check_path;
    if (num == 1)
    {
        std::string out{_dir.get_dir(std::format("test\\Task{}\\{}.out", id, id))};
        std::string ans{_dir.get_dir(std::format("test\\Task{}\\{}.ans", id, id))};
        check_path.emplace_back(std::make_pair(out, ans));
    }
    else
    {
        for (int i = 1; i <= num; ++i)
        {
            std::string out{_dir.get_dir(std::format("test\\Task{}\\{}.out", id, i))};
            std::string ans{_dir.get_dir(std::format("test\\Task{}\\{}.ans", id, i))};
            check_path.emplace_back(std::make_pair(out, ans));
        }
    }
    if (hashcheck(check_path))
    {
        std::cout << "Task" << id << " 测试通过！" << std::endl;
    }
    else
    {
        std::cout << "Task" << id << " 测试未通过！" << std::endl;
    }
    std::cout << std::endl;
}

bool Checker::hashcheck(const std::vector<std::pair<SafeString, SafeString>> &list)
{
    int cnt = 0;
    bool res = true;
    for (const auto &[out, ans] : list)
    {
        ++cnt;
        if (Filesys::compare(out, ans))
        {
            std::cout << "测试点" << cnt << " 通过！" << std::endl;
        }
        else
        {
            std::cout << "测试点" << cnt << " 未通过！" << std::endl;
            res = false;
        }
    }
    return res;
}

void Checker::all()
{
    for (int i = 1; i <= _proj.tasks.size(); ++i)
    {
        single(i, _proj.tasks["Task" + std::to_string(i)].nums);
    }
    if (_proj.result)
        getresultzip();
}

std::string Checker::gettaskhash(int id, int num)
{
    std::vector<SafeString> out_path;
    if (num == 1)
    {
        out_path.emplace_back(_dir.get_dir(std::format("test\\Task{}\\{}.out", id, id)));
    }
    else
    {
        for (int i = 1; i <= num; ++i)
        {
            out_path.emplace_back(_dir.get_dir(std::format("test\\Task{}\\{}.out", id, i)));
        }
    }
    std::string hash_data;
    for (const auto &item : out_path)
    {
        hash_data += Filesys::getSHA256(item);
    }
    SHA256 sha256;
    return sha256(hash_data);
}

void Checker::getresultzip()
{
    std::vector<SafeString> out_path;
    for (int id = 1; id <= _proj.tasks.size(); ++id)
    {
        int num = _proj.tasks["Task" + std::to_string(id)].nums;
        SafeString out = _dir.get_dir("test\\Task" + std::to_string(id) + ".out");
        SafeOfstream wt(out);
        wt << gettaskhash(id, num);
        wt.close();
        out_path.emplace_back(out);
    }

    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    mz_zip_writer_init_file(&zip, _dir.get_dir("result.zip"), 0);
    for (const auto &item : out_path)
    {
        std::filesystem::path p(item);
        std::string filename = p.filename().string();
        mz_zip_writer_add_file(&zip, filename.c_str(), item, nullptr, 0, MZ_BEST_COMPRESSION);
    }
    mz_zip_writer_finalize_archive(&zip);
    mz_zip_writer_end(&zip);
}

void Checker::run()
{
    std::cout << "指令：" << std::endl
              << "1.输入单个数字表示单次测试（例如想要测试Task1，输入1即可)" << std::endl
              << "2.输入all表示测试所有的测试点" << std::endl
              << "3.输入exit退出程序" << std::endl
              << std::endl;
    while (true)
    {
        std::cout << "输入指令：";
        std::string command;
        std::cin >> command;
        if (command == "all")
        {
            all();
        }
        else if (command == "exit")
        {
            exit(0);
        }
        else
        {
            auto is_number = [](const std::string &s) {
                return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
            };
            if (is_number(command))
            {
                int id = std::stoi(command);
                if (id <= 0)
                {
                    std::cout << "错误：最小的任务编号应该是1 而你输入了" << id << std::endl;
                }
                else if (id > _proj.tasks.size())
                {
                    std::cout << "错误：最大的任务编号应该是" << _proj.tasks.size() << " 而你输入了" << id << std::endl;
                }
                else
                {
                    single(id, _proj.tasks["Task" + command].nums);
                }
            }
            else
            {
                std::cout << "不要乱输入东西哦！" << std::endl;
            }
        }
    }
}