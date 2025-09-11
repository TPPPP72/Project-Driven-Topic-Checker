#pragma once

#include <iostream>

// 当入参为 false 时退出程序
inline void autoexit(bool x)
{
    if (!x)
    {
        std::cout << "按任意键退出" << std::endl;
        system("pause>nul");
        exit(0);
    }
}