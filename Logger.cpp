#include "Logger.h"

int Logger :: read_proc(){

    DIR *dir; 
    struct dirent *diread;

    for(auto & proc : current_running){ proc.second.safe = false;}
 
    if ((dir = opendir("/proc")) != nullptr) {
        time_t time_stamp = time(0);
       
        while ((diread = readdir(dir)) != nullptr) {
            if(is_number(diread->d_name)){ 

                int pid = stoi(diread->d_name);
                unordered_map<int,proc>::iterator iter = current_running.find(pid);
                if(iter != current_running.end()){
                    iter->second.etime = time_stamp;
                    iter->second.safe = true;
                }
                else{
                    proc temp;
                     temp.pid = pid;
                     temp.first_time = first_time;  
                     temp.etime = time_stamp;
                     temp.stime = time_stamp;
                     temp.safe = true; 
                     read_stat(&temp);   

                     current_running.insert({temp.pid, temp}); 

                     if(!first_time){
                        write_list.push_back(temp);
                     }
                }    
            }
        }
        closedir (dir);
    } else {
        perror ("opendir");
        return EXIT_FAILURE;
    } 

    for (unordered_map<int,proc>::iterator it = current_running.begin(); it != current_running.end(); ) {
        
        if (!it->second.first_time && !it->second.safe){
            write_list.push_back(it->second);
        }

        if(!it->second.safe){
            it = current_running.erase(it);
        }
        it++;
    }   

    first_time = false;

    if(counter == snapshot_interval){
        write_snapshot();
    }
    write_log();
    counter += seconds;
    usleep(1000 * 1000 * seconds);

    return 1;
}

int Logger :: read_stat(proc* p){

    string path = "/proc/"+to_string(p->pid)+"/stat";
    FILE* fp =  fopen(path.c_str(), "r");
    char buff[32];
    
    fscanf(fp, "%*d %s",buff);
    p->command = string(buff+1, strlen(buff)-2);    
    fclose(fp);

    struct stat info;
    stat(path.c_str(), &info); 
    p->gid = info.st_gid;
    p->uid = info.st_uid;
    return 1;   
}

bool Logger :: is_number(const std::string& s)
{
    string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int Logger :: write_log(){
    logFile.open("file.log", fstream::app);

    list<proc>::iterator it;
    for (it = write_list.begin(); it != write_list.end(); ++it){
        write(*it, logFile);        
    }
    logFile.close();
    write_list.clear();
}

int Logger :: write_snapshot(){

    snapshot_fs.open ("snapshot.log", fstream::app);
    for (unordered_map<int,proc>::iterator it = current_running.begin(); it != current_running.end(); ) {
        write(it->second, snapshot_fs);
        it++;
    } 
    snapshot_fs.close();
    return 1;
}

int Logger :: write(proc p, ofstream &fs){
    json j_object;
    if(p.safe){
        j_object = {{"pid", p.pid}, {"stime", p.stime}, {"name", p.command}, {"gid", p.gid}, {"uid", p.uid}};
    }
    else{
        j_object = {{"pid", p.pid}, {"stime", p.stime}, {"etime", p.etime}, {"name", p.command}, {"gid", p.gid}, {"uid", p.uid}};
    }
    
    fs << j_object.dump() << "\n";
    return 1;
}