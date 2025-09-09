# C++项目驱动题目检查器

## 使用教程

依据模板，为你需要检查的驱动题目写好 `checker.cpp` 以及 `config.json` 。 

自行为你的题目设置存放设置点的文件夹 `test` ，以下为一个题目可行的树形结构：


+ your_question
    + src1.cpp
    + src2.cpp
    + src3.cpp
    + config.json
    + test
        + Task1
            + 1.in
            + 1.ans
            + 2.in
            + 2.ans
        + Task2
            + 1.in
            + 1.ans
            + 2.in
            + 2.ans
        + Task3
            + 1.in
            + 1.ans
            + 2.in
            + 2.ans


而后是 `checker.cpp` 的模板：

```cpp
// checker.cpp

// 将所有的类对象权限全部设置为 public
#define private public
#define protected public
// 在这里包含需要包含的头文件
#undef private
#undef protected

#include <fstream>
#include <functional>
#include <iostream>
#include <vector>

template <typename T> std::vector<std::string> split(T &&s, char dot = '.')
{
    std::vector<std::string> res;
    std::string temp;
    for (const auto &item : s)
    {
        if (item == dot)
        {
            res.emplace_back(std::move(temp));
            temp.clear();
            continue;
        }
        temp += item;
    }
    if (!temp.empty())
    {
        res.emplace_back(std::move(temp));
    }
    return res;
}

class Test_File_RW
{
  public:
    Test_File_RW(int task_id, int times, std::function<void()> f);
    static std::string GetOutput(int task_id)
    {
        std::string out = "./test/Task" + std::to_string(task_id) + "/" + std::to_string(task_id) + ".out";
        return out;
    }
};

void test(const std::string &arg);

std::vector<std::function<void(int)>> tasks;

int main(int argc, char *argv[])
{
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i)
    {
        args.emplace_back(argv[i]);
    }

    // task 1
    tasks.emplace_back([](int times) {
        Test_File_RW(1, times, []() {
            // 添加你的测试代码
        });
    });

    // task 2
    tasks.emplace_back([](int times) {
        Test_File_RW(2, times, []() {
            // 添加你的测试代码
        });
    });

    // task ......

    for (const auto &item : args)
    {
        test(item);
    }
}

Test_File_RW::Test_File_RW(int task_id, int times, std::function<void()> f)
{
    for (int i = 1; i <= times; ++i)
    {
        std::string in = "./test/Task" + std::to_string(task_id) + "/" + std::to_string(i) + ".in";
        std::string out = "./test/Task" + std::to_string(task_id) + "/" + std::to_string(i) + ".out";
        std::ifstream fin(in);
        std::ofstream fout(out);
        auto cin_buf = std::cin.rdbuf();
        auto cout_buf = std::cout.rdbuf();
        std::cin.rdbuf(fin.rdbuf());
        std::cout.rdbuf(fout.rdbuf());
        f();
        std::cin.rdbuf(cin_buf);
        std::cout.rdbuf(cout_buf);
    }
}

void test(const std::string &arg)
{
    if (arg.find("Task") == std::string::npos)
    {
        return;
    }
    auto parts = split(arg, '_');
    int id = std::stoi(parts[0].substr(4));
    int times = std::stoi(parts[1]);
    tasks[id - 1](times);
    return;
}
```

`config.json` 的样例：

```json
{
    "CCList": [
        "$Compiler$ $Flag$ $FileList$ -o $checker$"
    ],
    "FileList": [
        "checker.cpp",
        "player.cpp",
        "monster.cpp",
        "global.cpp",
        "npc.cpp",
        "map.cpp",
        "item.cpp"
    ],
    "Flag": "-std=c++11 -O2 -static",
    "Result": true,
    "Task1": {
        "nums": 10
    },
    "Task10": {
        "nums": 10
    },
    "Task11": {
        "nums": 10
    },
    "Task12": {
        "nums": 10
    },
    "Task2": {
        "nums": 10
    },
    "Task3": {
        "nums": 10
    },
    "Task4": {
        "nums": 10
    },
    "Task5": {
        "nums": 10
    },
    "Task6": {
        "nums": 10
    },
    "Task7": {
        "nums": 1
    },
    "Task8": {
        "nums": 10
    },
    "Task9": {
        "nums": 10
    }
}

```

