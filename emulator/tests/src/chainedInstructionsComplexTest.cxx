#include "instructions/encode.hxx"
#include "instructions/LDI.hxx"
#include "instructions/ADD.hxx"
#include "instructions/SUBI.hxx"
#include "instructions/PUSH.hxx"
#include "instructions/POP.hxx"
#include "instructions/CP.hxx"
#include "instructions/BRNE.hxx"
#include "instructions/RJMP.hxx"
#include "instructions/NOP.hxx"

#include "cpu.hxx"
#include "memory.hxx"
#include "decoder.hxx"

#include <catch2/catch_all.hpp>

#include <iostream>
#include <print>

TEST_CASE("Full instruction chain including stack and branching")
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
    decoder.AddRule(std::make_unique<SUBIMatcher>(&ctx));
    decoder.AddRule(std::make_unique<PUSHMatcher>(&ctx));
    decoder.AddRule(std::make_unique<POPMatcher>(&ctx));
    decoder.AddRule(std::make_unique<CPMatcher>(&ctx));
    decoder.AddRule(std::make_unique<BRNEMatcher>(&ctx));
    decoder.AddRule(std::make_unique<RJMPMatcher>(&ctx));
    decoder.AddRule(std::make_unique<NOPMatcher>(&ctx));

    std::vector<uint8_t> program;

    auto emit = [&](uint16_t opcode) {
        program.push_back(ExtractLow(opcode));
        program.push_back(ExtractHigh(opcode));
    };

    emit(EncodeLDI(16, 0x10));  // R16 = 0x10
    emit(EncodePUSH(16));       // Push R16
    emit(EncodeLDI(17, 0x05));  // R17 = 0x05
    emit(EncodeADD(16, 17));    // R16 = R16 + R17 = 0x15
    emit(EncodePOP(18));        // R18 = 0x10
    emit(EncodeSUBI(16, 0x05)); // R16 = R16 - 0x05 = 0x10
    emit(EncodeCP(16, 18));     // Compare R16 and R18 (should be equal)
    emit(EncodeBRNE(2));        // Should NOT jump, falls through
    emit(EncodeLDI(19, 0x42));  // Executes: R19 = 0x42
    emit(EncodeRJMP(2));        // Skip next
    emit(EncodeLDI(19, 0xFF));  // Should be skipped
    emit(EncodeNOP());          // End

    controller.Write<FlashMemory>(0x0000, program);
    access.SetSPH(0x00);
    access.SetSPL(0x80); // Set stack pointer


    int32_t executed = 0;
    while (true)
    {
        uint16_t pc = cpu.GetPC();

        if (pc >= program.size())
            break;

        auto instrBytes = controller.Read<FlashMemory>(pc, 2);
        auto instr = decoder.Decode(instrBytes);

        if (!instr)
            break;

        instr->Execute();
        ++executed;

        std::println(std::cout, "{}", instr->Mnemonic());
        
        if (instr->Opcode() == 0x00)
            break;
    }

    REQUIRE(access.GetRegister(16) == 0x10); // Final R16 value after add/sub
    REQUIRE(access.GetRegister(17) == 0x05);
    REQUIRE(access.GetRegister(18) == 0x10); // POP restored 0x10
    REQUIRE(access.GetRegister(19) == 0x42); // Confirm correct path taken
    REQUIRE(cpu.GetPC() == 24);              // 12 x 2 bytes
}