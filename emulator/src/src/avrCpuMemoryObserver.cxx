#include "avrCpuMemoryObserver.hxx"







SREGObserver::SREGObserver(AVRCpu* cpu)
    : cpu(cpu)
{
    if (!cpu)
        throw std::invalid_argument("CPU pointer cannot be null");
}

void SREGObserver::NotifyWriteRequested(Memory* mem, uint64_t address, const std::vector<unsigned char>& data)
{
    if (address == 0x5F && data.size() == 1)
        cpu->SetSREG(data[0]);
}

SPObserver::SPObserver(AVRCpu* cpu, bool high)
    : cpu(cpu), highByte(high)
{
    if (!cpu)
        throw std::invalid_argument("CPU pointer cannot be null");
}

void SPObserver::NotifyWriteRequested(Memory* mem, uint64_t address, const std::vector<unsigned char>& data)
{
    if (data.size() != 1)
        return;

    if (highByte && address == 0x3E)
        cpu->SetSPH(data[0]);
    else if (!highByte && address == 0x3D)
        cpu->SetSPL(data[0]);
}

GPRObserver::GPRObserver(AVRCpu* cpu)
    : cpu(cpu)
{
    if (!cpu)
        throw std::invalid_argument("CPU pointer cannot be null");
}

void GPRObserver::NotifyWriteRequested(Memory* mem, uint64_t address, const std::vector<unsigned char>& data)
{
    for (size_t i = 0; i < data.size(); ++i)
    {
        uint64_t regIndex = address + i;
        if (regIndex <= 0x1F)
            cpu->SetRegister(static_cast<uint8_t>(regIndex), data[i]);
    }
}