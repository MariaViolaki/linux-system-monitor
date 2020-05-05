#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int id);
  int Pid();
  std::string User();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  std::string Command();
  bool operator<(Process const& a) const;

 private:
  int id_{ 0 };
  float cpu_{ 0.0 };
};

#endif
