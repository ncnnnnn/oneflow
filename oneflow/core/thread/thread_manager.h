#ifndef ONEFLOW_CORE_THREAD_THREAD_MANAGER_H_
#define ONEFLOW_CORE_THREAD_THREAD_MANAGER_H_

#include <memory>
#include "oneflow/core/thread/thread.h"
#include "oneflow/core/actor/actor_message_bus.h"
#include "oneflow/core/common/channel.h"
#include "oneflow/core/common/protobuf.h"

namespace oneflow {

class ThreadMgr final {
 public:
  OF_DISALLOW_COPY_AND_MOVE(ThreadMgr);
  ~ThreadMgr();

  OF_SINGLETON(ThreadMgr);
  
  Thread* GetThrd(uint64_t thrd_loc_id);

 private:
  ThreadMgr();

  std::vector<std::unique_ptr<Thread>> threads_;
};

}  // namespace oneflow

#endif // ONEFLOW_CORE_THREAD_THREAD_MANAGER_H_
