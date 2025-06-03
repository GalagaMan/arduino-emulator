#include "instructions/BRNE.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>

TEST_CASE("BRNE executes conditional relative branch based on Z flag")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    BRNEMatcher matcher(&ctx);

    SECTION("Z flag clear — branch taken")
    {
        cpu.SetPC(0x100);
        access.SetSREG(0x00); // Z = 0
        std::vector<uint8_t> brneBytes = {0x01, 0xF4}; // BRNE +1

        REQUIRE(matcher.Matches(brneBytes));
        auto instr = matcher.Decode(brneBytes);
        REQUIRE(instr != nullptr);

        instr->Execute();
        REQUIRE(cpu.GetPC() == 0x102); // 0x100 + 2
    }

    SECTION("Z flag set — no branch")
    {
        cpu.SetPC(0x100);
        access.SetSREG(0x02); // Z = 1
        std::vector<uint8_t> brneBytes = {0x01, 0xF4}; // BRNE +1

        REQUIRE(matcher.Matches(brneBytes));
        auto instr = matcher.Decode(brneBytes);
        REQUIRE(instr != nullptr);

        instr->Execute();
        REQUIRE(cpu.GetPC() == 0x102); // 0x100 + 2 (no jump)
    }
}
