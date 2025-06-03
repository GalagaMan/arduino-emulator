#include "instructions/POP.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>

TEST_CASE("POP loads value from stack into register")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(512), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x100);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory),
                                      0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};


    access.SetSPH(0x00);
    access.SetSPL(0x80);
    controller.Write<SRAMMemory>(0x0080, {0xAB});


    POPMatcher matcher(&ctx);
    std::vector<uint8_t> popBytes = {0x0F, 0x91}; // POP R16

    REQUIRE(matcher.Matches(popBytes));
    auto instr = matcher.Decode(popBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();

    REQUIRE(static_cast<uint32_t>(access.GetRegister(16)) == 0xAB);
    // REQUIRE(((access.GetSPH() << 8) | access.GetSPL()) == 0x0101);
    REQUIRE(((access.GetSPH() << 8) | access.GetSPL()) == 0x0081);
    REQUIRE(cpu.GetPC() == 0x102);
}
