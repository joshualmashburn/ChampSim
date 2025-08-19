#ifndef CPI_PRINTOUT_H
#define CPI_PRINTOUT_H

#include <iostream>

#include "event_listeners.h"

namespace cpi_printout {
  
template<Event e, typename... Args>
inline void handle_event(Args... args) {
  std::cout << "generic handle event\n";
}

template<>
inline void handle_event<Event::BEGIN_PHASE>(bool is_warmup) {
  std::cout << "specialized handle event\n";
}

}

class CPIPrintout {
public:
  template<Event e, typename... Args>
  void handle_event(Args... args) {
    std::cout << "CPIPrintout class handle event\n";
    cpi_printout::handle_event<e>(args...);
  }
};

#endif