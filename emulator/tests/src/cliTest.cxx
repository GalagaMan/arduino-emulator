#include "instructions/CLI.hxx"
#include "cpu.hxx"
#include "decoder.hxx"
#include "instructions/encode.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>


TEST_CASE("CLI clears global interrupt flag in SREG")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(512), 0x0000);
    controller.RegisterMemoryAs<FlashMemory>(std::make_unique<FlashMemory>(512), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    cpu.SetPC(0x0000);

    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    AVRInstructionDecoder decoder;
    decoder.AddRule(std::make_unique<CLIMatcher>(&ctx));

    std::vector<uint8_t> program;
    auto emit = [&](uint16_t word) {
        program.push_back(ExtractLow(word));
        program.push_back(ExtractHigh(word));
    };

    emit(EncodeCLI());

    controller.Write<FlashMemory>(0x0000, program);
    access.SetSREG(0xFF); // All flags set including I

    auto instr = decoder.Decode(program);
    REQUIRE(instr != nullptr);
    instr->Execute();

    REQUIRE((access.GetSREG() & (1 << 7)) == 0); // I-bit cleared
}
