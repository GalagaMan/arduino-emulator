#include "instructions/CRI.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>



TEST_CASE("CPI instruction sets SREG flags correctly after compare")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    CPIMatcher matcher(&ctx);

    access.SetRegister(17, 0x42); // R17 = 0x42
    access.SetSREG(0x00);         // clear flags

    std::vector<uint8_t> cpiBytes = {0x22, 0x31}; // CPI R17, 0x12

    REQUIRE(matcher.Matches(cpiBytes));
    auto instr = matcher.Decode(cpiBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();

    REQUIRE(cpu.GetPC() == 2);
    REQUIRE(access.GetRegister(17) == 0x42);

    uint8_t sreg = access.GetSREG();
    REQUIRE((sreg & (1 << 1)) == 0); // Z flag (should be cleared)
    REQUIRE((sreg & (1 << 0)) == 0); // C flag (should be cleared)
}
