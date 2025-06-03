#include "instructions/JMP.hxx"
#include "cpu.hxx"
#include "decoder.hxx"
#include "instructions/LDI.hxx"
#include "instructions/encode.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>

#include <print>
#include <iostream>

TEST_CASE("JMP jumps to target address")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(512), 0x0000);
    controller.RegisterMemoryAs<FlashMemory>(std::make_unique<FlashMemory>(512), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x0000);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    AVRInstructionDecoder decoder;
    decoder.AddRule(std::make_unique<JMPMatcher>(&ctx));
    decoder.AddRule(std::make_unique<LDIMatcher>(&ctx));

    std::vector<uint8_t> program;
    auto emit = [&](uint16_t word) {
        program.push_back(ExtractLow(word));
        program.push_back(ExtractHigh(word));
    };

    emit(EncodeJMPHigh(0x0006));
    emit(EncodeJMPLow(0x0006)); // 4 bytes: JMP to address 0x0006
    emit(EncodeLDI(16, 0xFF));  // This should be skipped
    emit(EncodeLDI(17, 0xAB));  // At 0x0006

    controller.Write<FlashMemory>(0x0000, program);
    cpu.SetPC(0x0000);

    for (int i = 0; i < 2; ++i)
    {
        uint16_t pc = cpu.GetPC();
        auto bytes2 = controller.Read<FlashMemory>(pc, 2);
        auto bytes4 = controller.Read<FlashMemory>(pc, 4);

        std::unique_ptr<Instruction> instr = decoder.Decode(bytes4);
        if (!instr)
            instr = decoder.Decode(bytes2);

        REQUIRE(instr != nullptr);
        instr->Execute();
    }

    REQUIRE(access.GetRegister(16) == 0x00); // Skipped
    REQUIRE(access.GetRegister(17) == 0xAB); // Executed
    REQUIRE(cpu.GetPC() == 0x0008);
}
