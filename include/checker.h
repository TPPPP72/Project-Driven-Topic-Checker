#pragma once

#include "config.h"

class Checker
{
  public:
    Checker(const Filesys::WorkingDir &dir, const Project &p)
    {
        _dir = dir;
        _proj = p;
    }
    void run();

  private:
    void all();
    void getresultzip();
    std::string gettaskhash(int id, int num);
    void single(int id, int num);
    bool hashcheck(const std::vector<std::pair<SafeString, SafeString>> &list);
    Filesys::WorkingDir _dir;
    Project _proj;
};