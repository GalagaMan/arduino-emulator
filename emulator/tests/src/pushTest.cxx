#include "instructions/PUSH.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>

TEST_CASE("PUSH pushes register onto stack")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x100);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    access.SetRegister(16, 0xAB); // R16
    access.SetSPH(0x01);
    access.SetSPL(0x00); // SP = 0x0100

    PUSHMatcher matcher(&ctx);
    std::vector<uint8_t> pushBytes = {0x0F, 0x93}; // PUSH R16

    REQUIRE(matcher.Matches(pushBytes));
    auto instr = matcher.Decode(pushBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();

    uint16_t newSP = (access.GetSPH() << 8) | access.GetSPL();
    REQUIRE(newSP == 0x00FF);

    auto popped = controller.Read<SRAMMemory>(0x00FF, 1);
    REQUIRE(popped.at(0) == 0xAB);

    REQUIRE(cpu.GetPC() == 0x102);
}
