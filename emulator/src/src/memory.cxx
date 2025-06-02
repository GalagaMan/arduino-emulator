#include "memory.hxx"
#include <ranges>






SRAMMemory::SRAMMemory(size_t size)
    : storage_(size, 0)
{}

void SRAMMemory::WriteLocation(uint64_t address, std::vector<unsigned char> const& data)
{
    if (address + data.size() > storage_.size())
        throw std::out_of_range("SRAMMemory write out of bounds");

    std::copy(data.begin(), data.end(), storage_.begin() + address);
}

std::vector<unsigned char> SRAMMemory::ReadLocation(uint64_t address, size_t size) const
{
    if (address + size > storage_.size())
        throw std::out_of_range("SRAMMemory read out of bounds");

    return {storage_.begin() + address, storage_.begin() + address + size};
}

size_t SRAMMemory::Size() const
{
    return storage_.size();
}

// --- FlashMemory Implementation ---
FlashMemory::FlashMemory(size_t size)
    : storage_(size, 0xFF) // Flash memory defaults to erased state 0xFF
{}

void FlashMemory::WriteLocation(uint64_t address, std::vector<unsigned char> const& data)
{
    if (address + data.size() > storage_.size())
        throw std::out_of_range("FlashMemory write out of bounds");

    for (size_t i = 0; i < data.size(); ++i)
    {
        size_t idx = address + i;

        // AVR flash can only flip bits from 1 to 0
        storage_[idx] &= data[i];
    }
}

std::vector<unsigned char> FlashMemory::ReadLocation(uint64_t address, size_t size) const
{
    if (address + size > storage_.size())
        throw std::out_of_range("FlashMemory read out of bounds");

    return {storage_.begin() + address, storage_.begin() + address + size};
}

size_t FlashMemory::Size() const
{
    return storage_.size();
}

void MemoryController::RegisterMemory(std::unique_ptr<Memory> memory, uint64_t start)
{
    auto const memorySize = memory->Size();
    auto const end = start + memorySize;
    auto* memoryPtr = memory.get();

    // Check for overlaps
    for (const auto& entry : memoryMap)
    {
        if (boost::icl::intersects(entry.first, boost::icl::interval<uint64_t>::right_open(start, end)))
            throw std::runtime_error("Memory overlap detected");
    }

    auto& memoryUnitsOfGivenType = memoryUnits[typeid(*memory)];
    memoryUnitsOfGivenType.emplace_back(std::move(memory));

    memoryMap += std::make_pair(boost::icl::interval<uint64_t>::right_open(start, end), memoryPtr);
}

void MemoryObserver::NotifyReadCompleted(Memory*, uint64_t, std::vector<unsigned char> const&)
{
}

void MemoryObserver::NotifyReadRequested(Memory*, uint64_t, size_t)
{
}

void MemoryObserver::NotifyWriteRequested(Memory*, uint64_t, std::vector<unsigned char> const&)
{
}

void MemoryObserver::NotifyWriteCompleted(Memory*, uint64_t)
{
}