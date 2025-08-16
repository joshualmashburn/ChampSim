
#include <iostream>
#include <map>
#include <string>

#include "event_listeners.h"

std::map<std::string, EventListener*(*)()>& get_registered_event_listeners() {
  static std::map<std::string, EventListener*(*)()> registered_event_listeners;
  return registered_event_listeners;
}

static std::vector<EventListener*> activated_event_listeners;

void register_event_listener(std::string name, EventListener*(*create_el)()) {
  get_registered_event_listeners()[name] = create_el;
}

void init_event_listeners(const std::vector<std::string>& requested_listeners) {
  for (std::string name : requested_listeners) {
    if (get_registered_event_listeners().count(name) == 1) {
      activated_event_listeners.push_back((*get_registered_event_listeners()[name])());
    } else {
      std::cout << "WARNING: requested listener \"" << name << "\" not found\n";
    }
  }
}

void cleanup_event_listeners() {
  for (EventListener* el : activated_event_listeners) {
    delete el;
  }
  activated_event_listeners.clear();
  
  // TODO: cleanup registered event listener constructors?
}

void handle_begin_phase(bool is_warmup) {
  for (EventListener* e : activated_event_listeners) {
    e->handle_begin_phase(is_warmup);
  }
}