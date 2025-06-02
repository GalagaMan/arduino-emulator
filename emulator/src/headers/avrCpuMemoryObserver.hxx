#pragma once
#include "memory.hxx"
#include "cpu.hxx"

class SREGObserver : public MemoryObserver
{
    AVRCpu* cpu;

public:
    SREGObserver(AVRCpu* cpu)
        : cpu(cpu)
    {
        if (!cpu)
            throw std::invalid_argument("CPU pointer cannot be null");
    }

    void NotifyWriteRequested(Memory* mem, uint64_t address, const std::vector<unsigned char>& data) override
    {
        if (address == 0x5F && data.size() == 1)
            cpu->SetSREG(data[0]);
    }
};

class SPObserver : public MemoryObserver
{
    AVRCpu* cpu;
    bool highByte;

public:
    SPObserver(AVRCpu* cpu, bool high)
        : cpu(cpu), highByte(high)
    {
        if (!cpu)
            throw std::invalid_argument("CPU pointer cannot be null");
    }

    void NotifyWriteRequested(Memory* mem, uint64_t address, const std::vector<unsigned char>& data) override
    {
        if (data.size() != 1)
            return;

        if (highByte && address == 0x3E)
            cpu->SetSPH(data[0]);
        else if (!highByte && address == 0x3D)
            cpu->SetSPL(data[0]);
    }
};