#include <catch2/catch_test_macros.hpp>
#include "cpu.hxx"
#include "memory.hxx"
#include "instructions/RJMP.hxx"




TEST_CASE("RJMP instruction performs relative jump correctly")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    RJMPMatcher matcher(&ctx);

    // Relative jump +2 → PC += 4 (offset is in words)
    std::vector<uint8_t> rjmpBytes = {0x02, 0xC0}; // RJMP +2

    cpu.SetPC(0x0100);

    REQUIRE(matcher.Matches(rjmpBytes));
    auto instr = matcher.Decode(rjmpBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();

    REQUIRE(cpu.GetPC() == 0x0104);
}
