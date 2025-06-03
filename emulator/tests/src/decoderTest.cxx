#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "cpu.hxx"
#include "instructions/EOR.hxx"
#include "instructions/NOP.hxx"
#include "memory.hxx"
#include "instruction.hxx"


TEST_CASE("NOP instruction decodes and executes correctly")
{
    MemoryController memController;
    AVRCpu cpu(16'000'000, &memController);
    AVRInstructionContext ctx{&cpu, nullptr, nullptr};
    NOPMatcher matcher(&ctx);

    std::vector<uint8_t> bytes = {0x00, 0x00}; // NOP opcode

    REQUIRE(matcher.Matches(bytes));
    auto instr = matcher.Decode(bytes);

    uint16_t oldPC = cpu.GetPC();
    instr->Execute();
    REQUIRE(cpu.GetPC() == oldPC + 2);
}
#include <print>
#include <iostream>

TEST_CASE("EOR instruction decodes and executes correctly")
{
    MemoryController memController;
    auto sram = std::make_unique<SRAMMemory>(256);
    memController.RegisterMemory(std::move(sram), 0x0000);

    AVRCpu cpu(16'000'000, &memController);
    memController.Write<SRAMMemory>(0x0001, {0b10101010}); // R1
    memController.Write<SRAMMemory>(0x0002, {0b11001100}); // R2

    MemoryBackedAccessBehavior regAccess(&memController, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D,
                                         0x3E);

    AVRInstructionContext ctx{&cpu, &regAccess, &memController};
    
    // ctx.accessBehavior = &regAccess;

    EORMatcher matcher(&ctx);

    // Encoding: 0010 0100 0001 0010 = 0x2400 + ((1 & 0x1F) << 4) + ((2 & 0x0F) + ((2 & 0x10) << 5))
    std::vector<uint8_t> bytes = {0x12, 0x24};

    REQUIRE(matcher.Matches(bytes));
    auto instr = matcher.Decode(bytes);
    cpu.SetPC(0x100);

    instr->Execute();

    REQUIRE(cpu.GetPC() == 0x102);
    auto result = memController.Read<SRAMMemory>(0x0001, 1);
    REQUIRE(result[0] == (0b10101010 ^ 0b11001100));
}

TEST_CASE("InstructionDecoder returns nullptr on unknown instruction")
{
    AVRInstructionDecoder decoder;
    std::vector<uint8_t> unknown = {0xFF, 0xFF}; // Invalid instruction

    auto instr = decoder.Decode(unknown);
    REQUIRE(instr == nullptr);
}

TEST_CASE("MemoryBackedAccessBehavior reads/writes registers correctly")
{
    MemoryController memController;
    auto sram = std::make_unique<SRAMMemory>(256);
    memController.RegisterMemory(std::move(sram), 0x0000);

    auto* cptr = &memController;

    MemoryBackedAccessBehavior behavior(&memController, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D,
                                        0x3E);

    behavior.SetRegister(5, 0x42);
    REQUIRE(behavior.GetRegister(5) == 0x42);

    behavior.SetSREG(0xAA);
    REQUIRE(behavior.GetSREG() == 0xAA);

    behavior.SetSPL(0x33);
    behavior.SetSPH(0x77);
    REQUIRE(behavior.GetSPL() == 0x33);
    REQUIRE(behavior.GetSPH() == 0x77);
}