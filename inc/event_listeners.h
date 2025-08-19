#ifndef EVENT_LISTENERS_H
#define EVENT_LISTENERS_H

#include <vector>
#include <string>
#include <iostream>
#include <tuple>

enum Event {
  BEGIN_PHASE,
  DO_PREDICT_BRANCH
};

#include "listeners/cpi_printout.h"
#include "listeners/test_listener.h"

inline std::vector<bool> listener_activation_map;
inline auto listeners = std::make_tuple(new CPIPrintout(), new TestListener());
inline std::vector<std::string> listener_names = {"CPIPrintout", "TestListener"};

inline void init_event_listeners(const std::vector<std::string>& requested_listeners) {
  listener_activation_map = std::vector<bool>(listener_names.size(), 0);
  for (std::string name : requested_listeners) {
    bool found = false;
    for (size_t i = 0; i < listener_names.size(); i++) {
      if (listener_names[i] == name) {
        listener_activation_map[i] = true;
        found = true;
        break;
      }
    }
    if (!found) {
      std::cout << "WARNING: Listener \"" << name << "\" not found\n";
    }
  }
}

template<Event e, typename... Args>
void handle_event(Args... args) {
  if (listener_activation_map[0]) {
    std::get<0>(listeners)->handle_event<e>(args...);
  }
  if (listener_activation_map[1]) {
    std::get<1>(listeners)->handle_event<e>(args...);
  }
}

#endif