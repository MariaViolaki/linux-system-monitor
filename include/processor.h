#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long jiffies_{ 0 };
  long active_jiffies_{ 0 };
  long idle_jiffies_{ 0 };
};

#endif
