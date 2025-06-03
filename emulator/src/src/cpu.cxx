#include "cpu.hxx"
#include "memory.hxx"
#include <stdexcept>
#include <format>
#include <cassert>


CPU::CPU(uint64_t speed, class MemoryController* memoryController)
    : speed(speed), memoryController(memoryController)
{
    if (speed == 0)
        throw std::invalid_argument("CPU speed cannot be zero");
}

AVRCpu::AVRCpu(uint64_t speed, class MemoryController* memoryController)
    : CPU(speed, memoryController)
{
}

