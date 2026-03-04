#include <catch.hpp>

#include "tracereader.h"

const std::string trace{{
    // Instruction 0
    '\x3a', '\x13', '\x00', '\x4c', '\x00', '\x00', '\x00', '\x00', // ip (offset 0)
    '\x00',                                                         // is branch (offset 8)
    '\x00',                                                         // branch taken (offset 9)
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',                 // padding (10-15)
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // branch_target (offset 16)
    '\x00', '\x3b',                                                 // destination registers (offset 24)
    '\x00', '\x00', '\x00', '\x00',                                 // source registers (offset 26)
    '\x00', '\x00',                                                 // padding (30-31)
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // dmem0 (offset 32)
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // dmem1 (offset 40)
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem0 (offset 48)
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem1 (offset 56)
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem2 (offset 64)
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem3 (offset 72)
    '\x00',                                                         // flags (offset 80)
    '\x00',                                                         // pref (offset 81)
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',                 // padding (82-87)

    // Instruction 1
    '\x3a', '\x16', '\x00', '\x4c', '\x00', '\x00', '\x00', '\x00', // ip
    '\x00',                                                         // is branch
    '\x00',                                                         // branch taken
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',                 // padding
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // branch_target
    '\x00', '\x49',                                                 // destination registers
    '\x00', '\x00', '\x00', '\x00',                                 // source registers
    '\x00', '\x00',                                                 // padding
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // dmem0
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // dmem1
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem0
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem1
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem2
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem3
    '\x00',                                                         // flags
    '\x00',                                                         // pref
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',                 // padding

    // Instruction 2
    '\x3a', '\x1c', '\x00', '\x4c', '\x00', '\x00', '\x00', '\x00', // ip
    '\x00',                                                         // is branch
    '\x00',                                                         // branch taken
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',                 // padding
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // branch_target
    '\x00', '\x11',                                                 // destination registers
    '\x00', '\x06', '\x00', '\x00',                                 // source registers
    '\x00', '\x00',                                                 // padding
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // dmem0
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // dmem1
    '\xe8', '\x58', '\x37', '\xb2', '\x7f', '\xfe', '\x00', '\x00', // smem0
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem1
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem2
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', // smem3
    '\x00',                                                         // flags
    '\x00',                                                         // pref
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00'                  // padding
}};

TEST_CASE("A tracereader can read the byte representation of an input_instr")
{
  champsim::bulk_tracereader<input_instr, std::istringstream> uut{0, std::istringstream{trace}};
  auto inst0 = uut();
  REQUIRE(inst0.ip == champsim::address{0x4c00133a});
  REQUIRE(inst0.is_branch == false);
  REQUIRE_THAT(inst0.destination_registers, Catch::Matchers::RangeEquals(std::vector{59}));
  REQUIRE_THAT(inst0.source_registers, Catch::Matchers::IsEmpty());
  REQUIRE_THAT(inst0.destination_memory, Catch::Matchers::IsEmpty());
  REQUIRE_THAT(inst0.source_memory, Catch::Matchers::IsEmpty());
  REQUIRE(inst0.is_wrong_path == false);

  auto inst1 = uut();
  REQUIRE(inst1.ip == champsim::address{0x4c00163a});
  REQUIRE(inst1.is_branch == false);
  REQUIRE_THAT(inst1.destination_registers, Catch::Matchers::RangeEquals(std::vector{73}));
  REQUIRE_THAT(inst1.source_registers, Catch::Matchers::IsEmpty());
  REQUIRE_THAT(inst1.destination_memory, Catch::Matchers::IsEmpty());
  REQUIRE_THAT(inst1.source_memory, Catch::Matchers::IsEmpty());
  REQUIRE(inst1.is_wrong_path == false);
}
