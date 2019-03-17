//
// scheduler.cc
//

#include "scheduler.h"

namespace simulation {

Task::Task(const Time time) : time(Scheduler::get_instance().time + time) {
  Scheduler::get_instance().ScheduleTask(this);
}

bool Task::operator<(const Task &other) const {
  return this->time < other.time;
}

bool Task::TaskComparer::operator()(const Task *t1, const Task *t2) {
  // Put items in reverse orser (ascending)
  return *t2 < *t1;
}

Scheduler& Scheduler::get_instance() {
  static Scheduler instance;  // Meyers singleton
  return instance;
}

void Scheduler::ScheduleTask(Task* task) {
  schedule_.push(task);
}

Time Timings::DeliveryDuration(const Node &from, const Node &to,
    size_t packet_size_bytes) {
  // This calculation is purely based on averagy wifi speed in Mbps and
  // packet size.
  // TODO: form nodes we can obtain also their distance via Position::Distance
  //       this could be used for more precise calculation.
  // Return value is in mili seconds.
  return (packet_size_bytes * 8) / signal_speed_Mbps * 1000;
}

} // namespace simulation