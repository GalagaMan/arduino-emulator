#include "instructions/BREQ.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>

TEST_CASE("BREQ instruction branches if zero flag is set")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x100);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    BREQMatcher matcher(&ctx);

    access.SetSREG(1 << 1); // Z flag set

    std::vector<uint8_t> breqBytes = {0x01, 0xF0}; // BREQ +1

    REQUIRE(matcher.Matches(breqBytes));
    auto instr = matcher.Decode(breqBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();

    REQUIRE(cpu.GetPC() == 0x102);
}

TEST_CASE("BREQ instruction does not branch if zero flag is clear")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x100);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    BREQMatcher matcher(&ctx);

    access.SetSREG(0x00); // Z flag clear

    std::vector<uint8_t> breqBytes = {0x01, 0xF0}; // BREQ +1

    REQUIRE(matcher.Matches(breqBytes));
    auto instr = matcher.Decode(breqBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();

    REQUIRE(cpu.GetPC() == 0x100 + 2); // normal PC increment
}
