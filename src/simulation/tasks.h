//
// tasks.h
//

#ifndef SARP_SIMULATION_TASKS_H_
#define SARP_SIMULATION_TASKS_H_

#include <memory>

#include "scheduler.h"
#include "node.h"

namespace simulation {

class SendTask : public Task {
 public:
  SendTask(const Time time, Node &sender,
      std::unique_ptr<ProtocolPacket> &&packet);
  ~SendTask() = default;

  void execute() override;
private:
  Node &sender_;
  std::unique_ptr<ProtocolPacket> packet_;
};

class RecvTask : public Task {
 public:
  RecvTask(const Time time, const Node &reciever,
      std::unique_ptr<ProtocolPacket> &&packet);
  ~RecvTask() = default;

  void execute() override;
private:
  Node &reciever_;
  std::unique_ptr<ProtocolPacket> packet_;
};

class ProcessTask : public Task {
 public:
  ProcessTask(const Time time, Node &node);
  ~ProcessTask() = default;

  void execute() override;
private:
  Node &node_;
};


}  // namespace simulation

#endif  // SARP_SIMULATION_TASKS_H_