#include "Proc_deamon.h"



class Logger{
    
    private:
    ofstream snapshot_fs;
    ofstream logFile;

    bool first_time = true;
    int counter = 0;
    bool snapshot = false;
     
    bool remove_and_write(proc p);

    bool is_number(const std::string& s);

    int write(proc p, ofstream &fs);

    int read_stat(proc* p);

    int write_log();

    int write_snapshot();

    unordered_map<int, proc> current_running;
    list<proc> write_list;
    
    public:
    int seconds = 1;
    int snapshot_interval = 30;
    
    int read_proc();

    



};