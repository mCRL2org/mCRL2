#include <algorithm>

#include "executor.h"
#include "process.h"

namespace squadt {

  void executor::handle_process_termination(executor_state_listener* l, process* p) {
    processes.erase(p);

    // start process for delayed command
    start_delayed();

    l->report_state_change(p.get_status());
  }

  void executor::execute(std::string command, executor_state_listener* l) {
    if (processes.size() < maximum_concurrent_processes) {
      start_process(command, l);
    }
    else {
      /* queue command for later execution */
      queue.push_back(std::pair < std::string, executor_state_listener* >(command, target));
    }
  }
}

