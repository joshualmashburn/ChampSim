#ifndef TEST_LISTENER_H
#define TEST_LISTENER_H

#include <iostream>

#include "event_listeners.h"

namespace test_listener {
  
template<Event e, typename... Args>
inline void handle_event(Args... args) {
  std::cout << "generic handle event\n";
}

template<>
inline void handle_event<Event::BEGIN_PHASE>(bool is_warmup) {
  std::cout << "specialized handle event\n";
}

}

class TestListener {
public:
  template<Event e, typename... Args>
  void handle_event(Args... args) {
    std::cout << "Test listener class handle event\n";
    test_listener::handle_event<e>(args...);
  }
};

#endif