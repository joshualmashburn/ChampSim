#ifndef EVENT_LISTENERS_H
#define EVENT_LISTENERS_H

#include <vector>
#include <string>
#include <iostream>

#define REGISTER(NAME) class Register ## NAME { \
public: \
  static EventListener* create_ ## NAME() { \
    return new NAME(); \
  } \
  Register ## NAME() { \
    register_event_listener(#NAME, Register ## NAME::create_ ## NAME); \
  } \
} inst_ ## NAME;

class EventListener {
public:
  virtual ~EventListener() {};
  
  virtual void handle_begin_phase(bool) {};
};

extern void register_event_listener(std::string name, EventListener*(*create_el)());
extern void init_event_listeners(const std::vector<std::string>& requested_listeners);
extern void cleanup_event_listeners();

extern void handle_begin_phase(bool);

#endif