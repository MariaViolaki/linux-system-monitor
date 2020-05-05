#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "system.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

// Read OS name from the filesystem
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

// Read OS kernel from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  string line, key, value, total_value, free_value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:")
        total_value = value;
      else if (key == "MemFree:")
        free_value = value;
      if (total_value != "" && free_value != "")
        break;
    }
    int total = stoi(total_value);
    int diff = total - stoi(free_value);
    float diff_percent = (100 * diff) / (float)total;
    return diff_percent / 100;
  }
  return 0.0;
}

// Search first value in stat file for a given key
int LinuxParser::SearchStatPair(string given_key) {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, key, value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == given_key) {
        return stoi(value);
      }
    }
  }
  return 0;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return SearchStatPair("processes");
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return SearchStatPair("procs_running");
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  string line, uptime;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return stol(uptime);
  }
  return 0;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  vector<string> cpu_values;
  string line, temp;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> temp;
    while (linestream >> temp) {
      cpu_values.push_back(temp);
    }
    return cpu_values;
  }
  return {};
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line, key, value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:")
        return value;
    }
  }
  return string();
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string given_id = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  string line, user, x, id;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> id;
      if (id == given_id)
        return user;
    }
  }
  return string();
}

// Read and return the cpu utilization percentage for a PID
float LinuxParser::ProcessCpu(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, value, utime, stime, cutime, cstime, starttime;
  float cpu_utilization = 0.0;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    for (int i = 1; i <= 22; i++) {
      std::istringstream linestream(line);
      linestream >> value;
      switch (i) {
        case 14:
          utime = value;
          break;
        case 15:
          stime = value;
          break;
        case 16:
          cutime = value;
          break;
        case 17:
          cstime = value;
          break;
        case 22:
          starttime = value;
          break;
      }
    }
    long total_time = stol(utime) + stol(stime) + stol(cutime) + stol(cstime);
    float seconds = System::uptime - (stol(starttime) / sysconf(_SC_CLK_TCK));
    cpu_utilization = (total_time / sysconf(_SC_CLK_TCK)) / seconds;
  }
  return cpu_utilization;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line, key, value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        return to_string(stol(value) / 1000);
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, value;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 1; i <= 22; i++) {
      linestream >> value;
    }
    return System::uptime - stol(value) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string();
}
