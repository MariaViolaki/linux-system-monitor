#include <string>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  seconds = seconds % 3600;
  int minutes = seconds / 60;
  seconds = seconds % 60;
  string time;
  if (hours < 10)
    time = "0" + to_string(hours) + ":";
  else
    time = to_string(hours) + ":";
  if (minutes < 10)
    time = time + "0" + to_string(minutes) + ":";
  else
    time = time + to_string(minutes) + ":";
  if (seconds < 10)
    time = time + "0" + to_string(seconds);
  else
    time = time + to_string(seconds);
  return time;
}
