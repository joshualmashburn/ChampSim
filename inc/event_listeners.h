#ifndef EVENT_LISTENERS_H
#define EVENT_LISTENERS_H

#include <vector>
#include <string>
#include <iostream>
#include <tuple>

#include "events.h"

#include "listeners/heartbeat.h"

inline std::vector<std::string> listener_names = {"Heartbeat"};
inline auto listeners = std::make_tuple(new Heartbeat());
inline std::bitset<std::tuple_size_v<decltype(listeners)>> listener_activation_map;

inline void init_event_listeners(const std::vector<std::string>& requested_listeners) {
  listener_activation_map.reset();
  listener_activation_map[0] = true; // heartbeat is always enabled
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

template<Event e, std::size_t Idx, typename... Args>
void handle_listener_event(Args&... args) {
    if (listener_activation_map[Idx]) {
        std::get<Idx>(listeners)->template handle_event<e>(args...);
    }
}

template<Event e, typename... Args>
void handle_event(Args&... args) {
    [&] <std::size_t... Is> (std::index_sequence<Is...>){
        (handle_listener_event<e, Is>(args...), ...);
    }(std::make_index_sequence<std::tuple_size_v<decltype(listeners)>>{}); // immediately invoked
}

#endif