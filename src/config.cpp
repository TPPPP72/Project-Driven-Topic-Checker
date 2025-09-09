#include "../include/config.h"

void to_json(nlohmann::json &j, const Task &t)
{
    j = nlohmann::json{{"nums", t.nums}};
}

void from_json(const nlohmann::json &j, Task &t)
{
    j.at("nums").get_to(t.nums);
}

void to_json(nlohmann::json &j, const Project &p)
{
    j = nlohmann::json{{"Compiler", p.Compiler},
                       {"Flag", p.Flag},
                       {"CCList", p.CClist},
                       {"FileList", p.Filelist},
                       {"Result", p.result}};
    for (auto &kv : p.tasks)
    {
        j[kv.first.str()] = kv.second;
    }
}

void from_json(const nlohmann::json &j, Project &p)
{
    p.Compiler = j.value("Compiler", "find");
    j.at("Flag").get_to(p.Flag);
    j.at("CCList").get_to(p.CClist);
    j.at("FileList").get_to(p.Filelist);
    j.at("Result").get_to(p.result);

    for (auto it = j.begin(); it != j.end(); ++it)
    {
        if (it.key().rfind("Task", 0) == 0)
        {
            p.tasks[it.key()] = it.value().get<Task>();
        }
    }
}