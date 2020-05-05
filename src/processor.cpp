#include "processor.h"
#include "linux_parser.h"

using LinuxParser::CPUStates;

// Return the aggregate CPU utilization
float Processor::Utilization() {
  std::vector<std::string> cpu_values = LinuxParser::CpuUtilization();
  long active_jiffies =
    stol(cpu_values[CPUStates::kUser_])
    + stol(cpu_values[CPUStates::kNice_])
    + stol(cpu_values[CPUStates::kSystem_])
    + stol(cpu_values[CPUStates::kIRQ_])
    + stol(cpu_values[CPUStates::kSoftIRQ_])
    + stol(cpu_values[CPUStates::kSteal_]);
  long idle_jiffies =
    stol(cpu_values[CPUStates::kIdle_])
    + stol(cpu_values[CPUStates::kIOwait_]);
  long jiffies = active_jiffies + idle_jiffies;

  float jiffies_percent = (active_jiffies - active_jiffies_)
    / (float)(jiffies - jiffies_);

  active_jiffies_ = active_jiffies;
  idle_jiffies_ = idle_jiffies;
  jiffies_ = jiffies;

  return jiffies_percent;
}
