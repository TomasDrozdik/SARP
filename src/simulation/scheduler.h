//
// scheduler.h
//

#ifndef SARP_SIMULATION_SCHEDULER_H_
#define SARP_SIMULATION_SCHEDULER_H_

#include <queue>
#include <memory>

#include <ctime>

#include "node.h"

namespace simulation {

using Time = unsigned long long;

class Task {
 public:

  class TaskComparer {
  public:
    TaskComparer() = default;
    bool operator()(const Task *lhs, const Task *rhs);
  };

  virtual ~Task() = 0;
  virtual void execute() = 0;

  bool operator<(const Task &other) const;

 protected:
  Task(const unsigned long long time);

  // Time in miliseconds form the beginning of the program
  const Time time;
};

class Scheduler {
 friend class Task;  // To let Task use ScheduleTask method and see current time
 public:
  // Meyers singleton
  static Scheduler& get_instance();

  Time get_current_time() const;

 private:

  Scheduler() = default;
  ~Scheduler() = default;

  // To preserve clean singleton
  Scheduler (const Scheduler&) = delete;
  Scheduler& operator=(const Scheduler&) = delete;

  void ScheduleTask(Task *task);

  Time time = 0;
  std::priority_queue<Task*, std::vector<Task*>, Task::TaskComparer> schedule_;
};

class Timings {
 public:
  static Time DeliveryDuration(const Node &from, const Node &to);
  static int get_node_process_factor();

 private:
  Timings() = delete;
  static const int signal_speed_Mbps = 200;
  static const int node_pocess_power_factor = 5;
};

} // namespace simulation

#endif  // SARP_SIMULATION_SCHEDULER_H_