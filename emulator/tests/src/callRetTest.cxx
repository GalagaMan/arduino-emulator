#include "cpu.hxx"
#include "decoder.hxx"
#include "instructions/CALL.hxx"
#include "instructions/LDI.hxx"
#include "instructions/RET.hxx"
#include "instructions/encode.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>

#include <iostream>

TEST_CASE("CALL jumps to subroutine and RET returns")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(512), 0x0000);
    controller.RegisterMemoryAs<FlashMemory>(std::make_unique<FlashMemory>(512), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x0000);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    AVRInstructionDecoder decoder;
    decoder.AddRule(std::make_unique<CALLMatcher>(&ctx));
    decoder.AddRule(std::make_unique<RETMatcher>(&ctx));
    decoder.AddRule(std::make_unique<LDIMatcher>(&ctx));

    std::vector<uint8_t> program;

    auto emit = [&](uint16_t opcode) {
        program.push_back(ExtractLow(opcode));
        program.push_back(ExtractHigh(opcode));
    };

    emit(EncodeCALLHigh(0x0006));  // bytes 2–3
    emit(EncodeCALLLow(0x0006));   // bytes 0–1
    emit(EncodeLDI(16, 0xAB));     // bytes 4–5
    emit(EncodeLDI(17, 0xCD));     // bytes 6–7  <-- this should execute first
    emit(EncodeRET());             // bytes 8–9

    controller.Write<FlashMemory>(0x0000, program);
    access.SetSPH(0x00);
    access.SetSPL(0x80); // Set stack pointer

    for (uint32_t i{}; i < 4; i++)
    {
        uint16_t pc = cpu.GetPC();
        std::vector<uint8_t> bytes2 = controller.Read<FlashMemory>(pc, 2);
        std::vector<uint8_t> bytes4 = controller.Read<FlashMemory>(pc, 4);

        std::unique_ptr<Instruction> instr = decoder.Decode(bytes4);
        if (!instr)
            instr = decoder.Decode(bytes2);

        REQUIRE(instr != nullptr);
        instr->Execute();
        std::println(std::cout, "{}", instr->Mnemonic());
    }

    REQUIRE(access.GetRegister(17) == 0xCD); // Set inside subroutine
    REQUIRE(access.GetRegister(16) == 0xAB); // Set after return
    REQUIRE(cpu.GetPC() == 0x06);              // Program counter after RET
}
