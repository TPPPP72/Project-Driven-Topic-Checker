#include "../include/config.h"

void to_json(nlohmann::json &j, const Task &t)
{
    j = nlohmann::json{{"nums", t.nums}};
    j = nlohmann::json{{"has_input", t.has_input}};
}

void from_json(const nlohmann::json &j, Task &t)
{
    j.at("nums").get_to(t.nums);
    t.has_input = j.value("has_input", true);
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
    p.Compiler = j.value("Compiler", "");
    p.Flag = j.value("Flag", "");
    j.at("CCList").get_to(p.CClist);
    j.at("FileList").get_to(p.Filelist);
    p.result = j.value("Result", true);

    for (auto it = j.begin(); it != j.end(); ++it)
    {
        if (it.key().rfind("Task", 0) == 0)
        {
            p.tasks[it.key()] = it.value().get<Task>();
        }
    }
}