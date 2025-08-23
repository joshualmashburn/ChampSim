#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

#include "event_listeners.h"
#include "instruction.h"

namespace debug {

template<Event e, typename... Args>
inline void handle_event(Args&... args) {
  std::cout << "WARNING: generic handle event\n";
}

template<>
inline void handle_event<Event::DO_PREDICT_BRANCH>(ooo_model_instr& instr, uint64_t& current_time) {
  std::cout << current_time << " do_predict_branch instr: " << instr.instr_id << " ip: " << instr.ip << " taken: " << instr.branch_taken << std::endl;
}

template<>
inline void handle_event<Event::DO_CHECK_DIB>(ooo_model_instr& instr, bool& is_hit, uint64_t& current_time) {
  std::cout << current_time << " do_check_dib instr: " << instr.instr_id << " ip: " << instr.ip << " is_hit: " << is_hit << std::endl;
}

}

class Debug {
private:
  bool in_warmup = false;
  
  template<typename... Args>
  void handle_begin_phase(bool& iw, Args&... args) {
    std::cout << "Begin phase; in_warmup = " << iw << std::endl;
    in_warmup = iw;
  }
  
public:
  template<Event e, typename... Args>
  void handle_event(Args&... args) {
    if constexpr (e == Event::BEGIN_PHASE) {
      handle_begin_phase(args...);
    } else if (!in_warmup) {
      debug::handle_event<e>(args...);
    }
  }
};

#endif