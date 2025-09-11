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

class Test_File_RW
{
  public:
    Test_File_RW(int task_id, int times, std::function<void()> f);
    static std::string GetOutput(int task_id)
    {
        std::string out = "./test/Task" + std::to_string(task_id) + "/" + std::to_string(1) + ".out";
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
    const char dot = '_';
    int id = std::stoi(arg.substr(0, arg.find(dot)));
    int times = std::stoi(arg.substr(arg.find(dot) + 1));
    tasks[id - 1](times);
    return;
}
```

`config.json` 的样例：  

这个样例指定了编译时需要包含 `checker.cpp` 文件以及 `player.cpp` 文件，同时开启了`O2` 优化，规定了编译的C++标准为 `C++11` ，且开启了静态编译。  

该样例有 $3$ 个任务：  
其中任务 $1$ 、 $2$ 具有输入文件，分别具有 $10$ 个以及 $5$ 个测试点；而任务 $3$ 不具有输入文件，只有 $1$ 个测试点。

```json
{
    "CCList": [
        "$Compiler$ $Flag$ $FileList$ -o $checker$"
    ],
    "FileList": [
        "checker.cpp",
        "player.cpp"
    ],
    "Flag": "-std=c++11 -O2 -static",
    "Task1": {
        "nums": 10
    },
    "Task2": {
        "nums": 5
    },
    "Task3": {
        "nums": 1,
        "has_input": false
    },
    "Result": true
}
```

