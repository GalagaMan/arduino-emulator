#include "cpu.hxx"
#include <stdexcept>
#include <format>
#include <cassert>


CPU::CPU(uint64_t speed) : speed(speed)
{
    if (speed == 0)
        throw std::invalid_argument("CPU speed cannot be zero");
}
