#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::ifstream memoFile(kProcDirectory + kMeminfoFilename);
  string line;

  if(memoFile.is_open()){
    float total, free;
    while(std::getline(memoFile, line)){
      string key, value;
      std::istringstream iss(line);
      iss >> key >> value;
      if(key.find("MemTotal")!=string::npos){
        total = std::stof(value);
      }
      if(key.find("MemFree")!=string::npos){
        free = std::stof(value);
        break;
      }
    }
    return (total-free) / total;
  }
  return 0;

 }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime;
  std::ifstream uptimeFile(kProcDirectory + kUptimeFilename);
  std::string line;
  if (std::getline(uptimeFile, line)) {
    std::istringstream iss(line);
    iss >> uptime;
  }
  uptimeFile.close();
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jiffies_list = CpuUtilization();
  return stol(jiffies_list[CPUStates::kUser_]) +
         stol(jiffies_list[CPUStates::kNice_]) +
         stol(jiffies_list[CPUStates::kSystem_]) +
         stol(jiffies_list[CPUStates::kIRQ_]) +
         stol(jiffies_list[CPUStates::kSoftIRQ_]) +
         stol(jiffies_list[CPUStates::kSteal_]) + 
         stol(jiffies_list[CPUStates::kIOwait_]) ;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffies_list = CpuUtilization();
  return stol(jiffies_list[CPUStates::kIdle_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> v;
  std::ifstream statFile(kProcDirectory + kStatFilename);
  string line;
  if (std::getline(statFile, line)) {
    std::istringstream iss(line);
    string cpu, value;
    iss >> cpu;
    while (iss >> value) v.push_back(value);
  }

  return v;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream processFile(kProcDirectory + kStatFilename);
  std::string line;
  int num = -1;
  while (std::getline(processFile, line)) {
    if (line.find("processes") != std::string::npos) {
      std::istringstream iss(line);
      std::string key;
      iss >> key >> num;
    }
  }
  return num;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream processFile(kProcDirectory + kStatFilename);
  std::string line;
  int num = -1;
  while (std::getline(processFile, line)) {
    if (line.find("procs_running") != std::string::npos) {
      std::istringstream iss(line);
      std::string key;
      iss >> key >> num;
    }
  }
  return num;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream cmdFile(LinuxParser::kProcDirectory + std::to_string(pid) +
                        LinuxParser::kCmdlineFilename);
  string line;
  while (std::getline(cmdFile, line)) {
    if (!line.empty()) break;
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream statusFile(LinuxParser::kProcDirectory + std::to_string(pid) +
                           LinuxParser::kStatusFilename);
  std::string line, ram;
  long int vmSize = -1;
  while (std::getline(statusFile, line)) {
    if (line.find("VmSize") != std::string::npos) {
      std::istringstream iss(line);
      std::string label;
      iss >> label >> vmSize;
      break;
    }
  }
  vmSize /= 1024;
  ram = std::to_string(vmSize);
  return ram;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream statusFile(LinuxParser::kProcDirectory + std::to_string(pid) +
                           LinuxParser::kStatusFilename);
  std::string line, uid;
  while (std::getline(statusFile, line)) {
    if (line.find("Uid") != std::string::npos) {
      std::istringstream iss(line.substr(4));
      vector<std::string> uid_list{};
      while (iss >> uid) {
        uid_list.push_back(uid);
      }
      if (!uid_list.empty()) {
        uid = uid_list[0];
      }
      break;
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::string uid = LinuxParser::Uid(pid);

  std::ifstream passwdFile(kPasswordPath);
  std::string line, user;
  while (std::getline(passwdFile, line)) {
    std::istringstream iss(line);
    std::string username, passwd, UID;
    std::getline(iss, username, ':');
    std::getline(iss, passwd, ':');
    std::getline(iss, UID, ':');
    if (uid == UID) {
      user = username;
      break;
    }
  }

  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long int LinuxParser::UpTime(int pid) {
  std::ifstream ifs(kProcDirectory + std::to_string(pid) + kStatFilename);
  string value{"0"};
  if (ifs.is_open()) {
    for (int i = 1; i < 23; i++) {
      ifs >> value;
    }
  }
  long proc_tck = std::stol(value);
  long proc_sec = proc_tck / sysconf(_SC_CLK_TCK);
  return proc_sec;
}
