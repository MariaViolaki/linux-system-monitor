#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();
  std::vector<Process>& Processes();
  std::string OperatingSystem();
  std::string Kernel();
  float MemoryUtilization();
  int TotalProcesses();
  int RunningProcesses();
  long UpTime();
  static long uptime;

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif
