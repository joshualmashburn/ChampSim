#include <catch.hpp>
#include <typeinfo>
#include <iostream>

#include "listeners/heartbeat.h"
#include "instruction.h"
#include "trace_instruction.h"

namespace
{

TEST_CASE("The heartbeat listener prints one line after 1M instructions retired") {
    std::ostringstream stdout{};
    Heartbeat uut{&stdout};
    for (int i = 0; i < 5000000; ++i) {
        std::deque<ooo_model_instr> fake_instructions{{ooo_model_instr(0, input_instr()), ooo_model_instr(0, input_instr())}};
        uint32_t cpu = 0;
        uint64_t curr_cycles = i;
        auto cb = std::cbegin(fake_instructions);
        auto ce = std::cend(fake_instructions);
        uut.handle_event<Event::RETIRE>(cpu, cb, ce, curr_cycles);
    }
    
    std::string res = stdout.str();
    
    REQUIRE(res.substr(0, res.find('(')) == "Heartbeat CPU 0 instructions: 10000000 cycles: 4999999 heartbeat IPC: 2 cumulative IPC: 2 ");
}

}