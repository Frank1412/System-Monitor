#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int p) {
  pid = p;
  command = LinuxParser::Command(pid);
  // cpuUtilization = LinuxParser::CpuUtilization();
  ram = LinuxParser::Ram(pid);
  user = LinuxParser::User(pid);
  uptime = LinuxParser::UpTime(pid);

  long total_uptime = LinuxParser::UpTime();
  long cur_uptime = LinuxParser::UpTime(pid);
  long cur_active = LinuxParser::ActiveJiffies(pid);
  cpuUtilization = float(cur_active) / (total_uptime - float(cur_uptime));

}

// TODO: Return this process's ID
int Process::Pid() { return pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpuUtilization; }

// TODO: Return the command that generated this process
string Process::Command() { return command; }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram; }

// TODO: Return the user (name) that generated this process
string Process::User() { return user; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}