#include "instructions/SBCI.hxx"
#include "cpu.hxx"
#include "decoder.hxx"
#include "instructions/LDI.hxx"
#include "instructions/encode.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>


TEST_CASE("SBCI subtracts immediate and carry from register")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(512), 0x0000);
    controller.RegisterMemoryAs<FlashMemory>(std::make_unique<FlashMemory>(512), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x0000);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    AVRInstructionDecoder decoder;
    decoder.AddRule(std::make_unique<LDIMatcher>(&ctx));
    decoder.AddRule(std::make_unique<SBCIMatcher>(&ctx));

    std::vector<uint8_t> program;

    auto emit = [&](uint16_t word) {
        program.push_back(ExtractLow(word));
        program.push_back(ExtractHigh(word));
    };

    emit(EncodeLDI(16, 0x20));
    emit(EncodeSBCI(16, 0x01));

    controller.Write<FlashMemory>(0x0000, program);
    access.SetSREG(0x01); // Carry flag set
    cpu.SetPC(0x0000);

    for (int i = 0; i < 2; ++i)
    {
        uint16_t pc = cpu.GetPC();
        auto bytes2 = controller.Read<FlashMemory>(pc, 2);
        auto instr = decoder.Decode(bytes2);
        REQUIRE(instr != nullptr);
        instr->Execute();
    }

    REQUIRE(access.GetRegister(16) == 0x1E); // 0x20 - 0x01 - 0x01
}
