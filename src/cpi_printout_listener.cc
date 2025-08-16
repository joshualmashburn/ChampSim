
#include <iostream>

#include "event_listeners.h"

class CPIPrintout : public EventListener {
public:
  
  void handle_begin_phase(bool is_warmup) {
    std::cout << "CPIPrintout handle begin phase " << is_warmup << std::endl;
  }
};

REGISTER(CPIPrintout)

/*class RegisterCPIPrintout {
public:
  static EventListener* create_CPIPrintout() {
    return new CPIPrintout();
  }
  RegisterCPIPrintout() {
    int testPizza = 5;
    std::cout << "constructor for RegisterCPIPrintout\n";
    std::cout << TEST(Pizza) << std::endl;
    register_event_listener("CPIPrintout", RegisterCPIPrintout::create_CPIPrintout);
  }
} inst_CPIPrintout; */