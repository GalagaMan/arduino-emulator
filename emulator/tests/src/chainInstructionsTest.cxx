#include <catch2/catch_all.hpp>
#include "cpu.hxx"
#include "memory.hxx"
#include "instructions/ADD.hxx"
#include "instructions/POP.hxx"
#include "instructions/PUSH.hxx"
#include "instructions/SUBI.hxx"
#include "instructions/LDI.hxx"




/* TEST_CASE("Chained instruction execution modifies state correctly") {
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(512), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x100);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory),
                                      0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access, &controller};

    access.SetSPH(0x01);
    access.SetSPL(0xFF);

    std::vector<std::vector<uint8_t>> programBytes = {
        {0x20, 0xE0},  // LDI R16, 0x20
        {0x10, 0xE1},  // LDI R17, 0x10
        {0x11, 0x0C},  // ADD R16, R17
        {0x0F, 0x92},  // PUSH R16
        {0x00, 0xE0},  // LDI R16, 0x00
        {0x0F, 0x91},  // POP R16
        {0x10, 0x50}   // SUBI R16, 0x10
    };

    std::vector<std::unique_ptr<Instruction>> instructions;
    AVRInstructionDecoder decoder;
    decoder.AddRule(std::make_unique<LDIMatcher>(&ctx));
    decoder.AddRule(std::make_unique<ADDMatcher>(&ctx));
    decoder.AddRule(std::make_unique<PUSHMatcher>(&ctx));
    decoder.AddRule(std::make_unique<POPMatcher>(&ctx));
    decoder.AddRule(std::make_unique<SUBIMatcher>(&ctx));

    for (auto& bytes : programBytes) {
        auto instr = decoder.Decode(bytes);
        REQUIRE(instr != nullptr);
        instructions.emplace_back(std::move(instr));
    }

    for (auto& instr : instructions) {
        instr->Execute();
    }

    REQUIRE(static_cast<uint32_t>(access.GetRegister(16)) == 0x20);  // Final expected R16
    REQUIRE(static_cast<uint32_t>(access.GetRegister(17)) == 0x10);  // R17 should remain unchanged
} */

#include "instructions/LDI.hxx"
#include "instructions/ADD.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include "decoder.hxx"
#include <catch2/catch_all.hpp>
#include <iostream>

TEST_CASE("Chained instruction execution modifies state correctly")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(512), 0x0000);
    controller.RegisterMemoryAs<FlashMemory>(std::make_unique<FlashMemory>(512), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x0000);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory),
                                      0x0000, 0x5F, 0x3D, 0x3E);

    AVRInstructionContext ctx{&cpu, &access};

    AVRInstructionDecoder decoder;
    decoder.AddRule(std::make_unique<LDIMatcher>(&ctx));
    decoder.AddRule(std::make_unique<ADDMatcher>(&ctx));

    std::vector<uint8_t> program = {
        0x20, 0xE0, // LDI R18, 0
        0x10, 0xE1, // LDI R17, 16
        0x11, 0x0E  // ADD R1, R17
    };

    controller.Write<FlashMemory>(0x0000, program);

    for (int i = 0; i < 3; ++i)
    {
        uint16_t pc = cpu.GetPC();
        std::vector<uint8_t> instrBytes = controller.Read<FlashMemory>(pc, 2);
        auto instr = decoder.Decode(instrBytes);
        REQUIRE(instr != nullptr);
        auto v = instr->Mnemonic();
        std::cout << v << "\n";
        instr->Execute();
    }

    REQUIRE(static_cast<uint32_t>(access.GetRegister(18)) == 0x00); // R18 = 0
    REQUIRE(static_cast<uint32_t>(access.GetRegister(17)) == 0x10); // R17 = 16
    REQUIRE(static_cast<uint32_t>(access.GetRegister(1)) == 0x10);  // R1 = 0 + 16
    REQUIRE(cpu.GetPC() == 6);
}
