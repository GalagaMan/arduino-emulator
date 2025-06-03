#include <catch2/catch_all.hpp>
#include "instructions/LDI.hxx"
#include "instruction.hxx"
#include "memory.hxx"
#include "cpu.hxx"






TEST_CASE("LDI instruction loads immediate into register") {
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory),
                                      0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    LDIMatcher matcher(&ctx);
    std::vector<uint8_t> ldiBytes = { 0x0F, 0xEF }; // LDI R16, 0xFF

    REQUIRE(matcher.Matches(ldiBytes));
    auto instr = matcher.Decode(ldiBytes);
    REQUIRE(instr != nullptr);

    instr->Execute();
    REQUIRE(access.GetRegister(16) == 0xFF);
    REQUIRE(cpu.GetPC() == 2);
}