#include "instructions/CP.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>

TEST_CASE("CP instruction sets flags correctly after compare")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x100);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    CPMatcher matcher(&ctx);

    access.SetRegister(16, 0x42); // Rd = R16
    access.SetRegister(17, 0x42); // Rr = R17
    access.SetSREG(0x00);

    std::vector<uint8_t> cpBytes = {0x11, 0x14}; // CP R16, R17

    REQUIRE(matcher.Matches(cpBytes));
    auto instr = matcher.Decode(cpBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();

    REQUIRE(cpu.GetPC() == 0x100 + 2);

    uint8_t sreg = access.GetSREG();

    REQUIRE((sreg & (1 << 1)));     // Z set
    REQUIRE_FALSE(sreg & (1 << 0)); // C clear
}
