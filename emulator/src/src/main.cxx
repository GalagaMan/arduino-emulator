#include <iostream>
#include <print>


int main()
{

    std::println(std::cout, "Hello World");

    return 0;

    size_t const rRegistersBegin = 0x0000;
    size_t const rRegistersEnd = 0x001F;
    size_t const rRegistersSize = rRegistersEnd - rRegistersBegin + 1;

    size_t const ioRegistersBegin = 0x0020;
    size_t const ioRegistersEnd = 0x005F;
    size_t const ioRegistersSize = ioRegistersEnd - ioRegistersBegin + 1;

    // LDS STS
    size_t const extIoRegistersBegin = 0x0060;
    size_t const extIoRegistersEnd = 0x00FF;
    size_t const extIoRegistersSize = extIoRegistersEnd - extIoRegistersBegin + 1;

    size_t const sramBegin = 0x0100;
    size_t const sramEnd = 0x08FF;
    size_t const sramSize = sramEnd - sramBegin + 1;

    size_t const startingResetVectorAddress = 0x0000;
    size_t const startingStackAddress = 0x08FF;

    // std::println(std::cout, "{} {}", 1024 * 2, ramSize);

    // RAM ram(ramSize);

    return EXIT_SUCCESS;
}