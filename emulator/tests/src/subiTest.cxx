#include <catch2/catch_all.hpp>
#include "memory.hxx"
#include "cpu.hxx"
#include "instructions/SUBI.hxx"


TEST_CASE("SUBI instruction subtracts immediate from register")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    access.SetRegister(18, 0x22); // R18
    SUBIMatcher matcher(&ctx);

    std::vector<uint8_t> subiBytes = {0x22, 0x51}; // SUBI R18, 0x12

    REQUIRE(matcher.Matches(subiBytes));
    auto instr = matcher.Decode(subiBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();

    REQUIRE(static_cast<int32_t>(access.GetRegister(18)) == 0x10);
    REQUIRE(cpu.GetPC() == 2);
}