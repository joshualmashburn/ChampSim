
#include <iostream>

#include "event_listeners.h"

class RetireCPIStack : public EventListener {
public:
  
  void handle_begin_phase(bool is_warmup) {
    std::cout << "RetireCPIStack handle begin phase " << is_warmup << std::endl;
  }
};

REGISTER(RetireCPIStack)

/*class RegisterRetireCPIStack {
public:
  static EventListener* create_RetireCPIStack() {
    return new RetireCPIStack();
  }
  RegisterRetireCPIStack() {
    std::cout << "constructor for RegisterRetireCPIStack\n";
    register_event_listener("RetireCPIStack", RegisterRetireCPIStack::create_RetireCPIStack);
  }
} inst_RetireCPIStack;*/