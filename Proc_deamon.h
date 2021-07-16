#include <iostream>
#include <dirent.h>
#include <string>
#include <algorithm>
#include <list>
#include <unistd.h>
#include <fstream>
#include <unordered_map>

#include <sys/stat.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

struct{
    int pid;
    string command;
    time_t stime;
    time_t etime;
    bool first_time;
    bool safe;
    int uid;
    int gid;

}typedef proc;
