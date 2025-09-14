#include "../include/config.h"

void to_json(nlohmann::json &j, const Task &t)
{
    j = nlohmann::json{{"test_point_name", t.test_point_name}};
    j = nlohmann::json{{"nums", t.nums}};
    j = nlohmann::json{{"has_input", t.has_input}};
    j = nlohmann::json{{"has_output", t.has_output}};
}

void from_json(const nlohmann::json &j, Task &t)
{
    t.nums = j.value("nums", 10);
    t.test_point_name = j.value("test_point_name", "");
    t.has_input = j.value("has_input", true);
    t.has_output = j.value("has_output", true);
}

void to_json(nlohmann::json &j, const Project &p)
{
    j = nlohmann::json{{"Compiler", p.Compiler},
                       {"Flag", p.Flag},
                       {"CCList", p.CClist},
                       {"FileList", p.Filelist},
                       {"Generate_result", p.Generate_result}};
    for (auto &kv : p.tasks)
    {
        j[kv.first.str()] = kv.second;
    }
}

void from_json(const nlohmann::json &j, Project &p)
{
    p.Compiler = j.value("Compiler", "");
    p.Flag = j.value("Flag", "");
    j.at("CCList").get_to(p.CClist);
    j.at("FileList").get_to(p.Filelist);
    p.Task_name = j.value("Task_name", "Task");
    p.Test_dir = j.value("Test_dir", "test");
    p.Generate_result = j.value("Generate_result", true);

    for (auto it = j.begin(); it != j.end(); ++it)
    {
        if (it.key().rfind(p.Task_name.str(), 0) == 0)
        {
            p.tasks[it.key()] = it.value().get<Task>();
        }
    }
}