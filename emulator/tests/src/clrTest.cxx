#include "instructions/CLR.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <catch2/catch_all.hpp>


TEST_CASE("CLR instruction clears register and sets flags")
{
    MemoryController controller;
    controller.RegisterMemoryAs<SRAMMemory>(std::make_unique<SRAMMemory>(256), 0x0000);

    AVRCpu cpu(16'000'000, &controller);
    MemoryBackedAccessBehavior access(&controller, typeid(SRAMMemory), typeid(FlashMemory), 0x0000, 0x5F, 0x3D, 0x3E);
    AVRInstructionContext ctx{&cpu, &access};

    access.SetRegister(3, 0xFF);
    access.SetSREG(0x00);
    cpu.SetPC(0x20);

    CLR clr(&ctx, 3);
    clr.Execute();

    REQUIRE(access.GetRegister(3) == 0);
    REQUIRE((access.GetSREG() & (1 << 1)) != 0); // Z
    REQUIRE((access.GetSREG() & (1 << 2)) == 0); // N
    REQUIRE(cpu.GetPC() == 0x22);
}