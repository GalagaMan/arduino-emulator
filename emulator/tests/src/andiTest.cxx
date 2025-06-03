#include "instructions/ANDI.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <catch2/catch_test_macros.hpp>


TEST_CASE("ANDI instruction performs bitwise AND with immediate")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    access.SetRegister(18, 0b11110000); // R18
    ANDIMatcher matcher(&ctx);

    std::vector<uint8_t> andiBytes = {0x20, 0x74}; // ANDI R18, 0x40 (0b01000000)

    REQUIRE(matcher.Matches(andiBytes));
    auto instr = matcher.Decode(andiBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();

    REQUIRE(static_cast<uint32_t>(access.GetRegister(18)) == (0b11110000 & 0b01000000)); // should be 0x40
    REQUIRE(cpu.GetPC() == 2);

    uint8_t sreg = access.GetSREG();
    REQUIRE((sreg & (1 << 1)) == 0); // Z should be 0
    REQUIRE((sreg & (1 << 2)) == 0); // N should be 0
    REQUIRE((sreg & (1 << 3)) == 0); // V should be 0
    REQUIRE((sreg & (1 << 4)) == 0); // S should be 0
}
