#include "instructions/LDI.hxx"
#include "instructions/ADD.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include "decoder.hxx"
#include <catch2/catch_all.hpp>
#include <iostream>

static std::vector<uint8_t> EncodeAddInstruction(uint8_t Rd, uint8_t Rr)
{
    // Format: 0000 11rd dddd rrrr
    // Bits 9-5: ddddd (destination)
    // Bit 4:    r bit 4 (high bit of Rr)
    // Bits 3-0: rrrr (low 4 bits of Rr)

    uint16_t word = 0;
    word |= 0b0000110000000000; // base opcode for ADD
    word |= ((Rd & 0x1F) << 4);
    word |= (Rr & 0x0F);
    word |= ((Rr & 0x10) << 5); // move high bit of Rr into bit 9

    return {
        static_cast<uint8_t>(word & 0xFF),       // low byte
        static_cast<uint8_t>((word >> 8) & 0xFF) // high byte
    };
}

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
        0x11, 0xE1,
        0x21, 0xE0
    };

    for(auto elem : EncodeAddInstruction(17, 18))
        program.emplace_back(elem);

    controller.Write<FlashMemory>(0x0000, program);

    for (int i = 0; i < 3; ++i)
    {
        uint16_t pc = cpu.GetPC();
        std::vector<uint8_t> instrBytes = controller.Read<FlashMemory>(pc, 2);
        auto instr = decoder.Decode(instrBytes);
        REQUIRE(instr != nullptr);
        instr->Execute();
    }

    REQUIRE(static_cast<uint32_t>(access.GetRegister(17)) == 18);  
    REQUIRE(cpu.GetPC() == 6);
}
