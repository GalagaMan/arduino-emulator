#include <catch2/catch_test_macros.hpp>
#include "cpu.hxx"
#include "memory.hxx"
#include "instructions/ORI.hxx"

TEST_CASE("ORI instruction performs bitwise OR with immediate and sets flags correctly")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    access.SetRegister(17, 0x22); // R17
    ORIMatcher matcher(&ctx);

    std::vector<uint8_t> oriBytes = {0x12, 0x61}; // ORI R17, 0x12
    REQUIRE(matcher.Matches(oriBytes));

    auto instr = matcher.Decode(oriBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();

    REQUIRE(static_cast<uint32_t>(access.GetRegister(17)) == (0x22 | 0x12)); // Expect 0x32
    REQUIRE(cpu.GetPC() == 2);

    uint8_t sreg = access.GetSREG();
    REQUIRE((sreg & (1 << 1)) == 0); // Z = 0
    REQUIRE((sreg & (1 << 2)) == 0); // N = 0
    REQUIRE((sreg & (1 << 3)) == 0); // V = 0
    REQUIRE((sreg & (1 << 4)) == 0); // S = 0
}
