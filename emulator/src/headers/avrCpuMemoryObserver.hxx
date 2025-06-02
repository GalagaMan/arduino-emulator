#pragma once
#include "memory.hxx"
#include "cpu.hxx"

class SREGObserver : public MemoryObserver
{
    AVRCpu* cpu;

public:
    SREGObserver(AVRCpu* cpu);
    
    void NotifyWriteRequested(Memory* mem, uint64_t address, const std::vector<unsigned char>& data) override;
};

class SPObserver : public MemoryObserver
{
    AVRCpu* cpu;
    bool highByte;

public:
    SPObserver(AVRCpu* cpu, bool high);

    void NotifyWriteRequested(Memory* mem, uint64_t address, const std::vector<unsigned char>& data) override;
};

class GPRObserver : public MemoryObserver
{
    AVRCpu* cpu;

public:
    GPRObserver(AVRCpu* cpu);

    void NotifyWriteRequested(Memory* mem, uint64_t address, const std::vector<unsigned char>& data) override;
};