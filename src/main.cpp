#include "../include/checker.h"
#include "../include/compile.hpp"
#include "../include/config.h"
#include "../include/exit.hpp"
#include "../include/win_safe_string.hpp"
#include <iostream>
#include <thread>

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "输入你的工作目录：";
    Filesys::WorkingDir dir;
    dir.getline(std::cin);
    std::cout << std::endl;

    std::cout << "检查配置文件......" << std::endl;
    Config conf(dir, "config.json");
    conf.init();
    auto q = conf.getproject();
    std::cout << "配置文件检查通过！" << std::endl << std::endl;

    auto compiler = q.Compiler;
    if (q.Compiler.empty())
    {
        std::cout << "配置文件未指定编译器，正在获取......" << std::endl;
        q.Compiler = get_compiler();
        autoexit(!q.Compiler.empty());
        std::cout << "编译器获取成功！" << std::endl << std::endl;
    }
    else
    {
        std::cout << "配置文件指定编译器，检查路径中......" << std::endl;
        if (Filesys::exists(q.Compiler))
        {
            std::cout << "检查通过！" << std::endl << std::endl;
            set_temp_ev(q.Compiler);
        }
        else
        {
            std::cout << "检查失败！正在尝试获取编译器......" << std::endl;
            q.Compiler = get_compiler();
            autoexit(!q.Compiler.empty());
            std::cout << "编译器获取成功！" << std::endl << std::endl;
        }
    }

    std::cout << "检查源文件文件列表......" << std::endl;
    for (const auto &item : q.Filelist)
    {
        autoexit(Filesys::exists(dir.get_dir(item)));
    }
    std::cout << "源文件列表检查通过！" << std::endl << std::endl;

    std::cout << "检查测试点数据......" << std::endl;
    for (auto &[k, v] : q.tasks)
    {
        autoexit(Filesys::exists(dir.get_dir("test\\" + k.str())));
        for (int i = 1; i <= v.nums; ++i)
        {
            if (v.has_input)
            {
                autoexit(Filesys::exists(dir.get_dir("test\\" + k.str() + "\\" + std::to_string(i) + ".in")));
            }
            autoexit(Filesys::exists(dir.get_dir("test\\" + k.str() + "\\" + std::to_string(i) + ".ans")));
        }
    }
    std::cout << "测试点数据检查通过！" << std::endl << std::endl;

    std::cout << "尝试编译checker.cpp......" << std::endl;
    for (const auto &item : Compiler{q}.run(dir))
    {
        if (item.exit_code != 0)
        {
            std::cout << "编译失败！请根据错误提示修改代码！" << std::endl << std::endl;
            std::cout << item.error << std::endl;
            std::cout << "按任意键退出程序" << std::endl;
            system("pause>nul");
        }
        else
        {
            std::cout << "编译成功！3秒后进入检查页面......" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            system("cls");
            Checker{dir, q}.run();
        }
    }
    system("pause>nul");
}