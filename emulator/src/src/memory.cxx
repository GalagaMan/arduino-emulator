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

FlashMemory::FlashMemory(size_t size)
    : storage_(size, 0xFF)
{
}

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

void MemoryController::RegisterMemory(std::unique_ptr<Memory> memory, uint64_t base)
{
    if (!memory)
        throw std::invalid_argument("Null memory");

    const std::type_index dynType = typeid(*memory);
    Memory* raw = memory.get();

    auto& mapPtr = memoryMaps[dynType];
    if (!mapPtr)
        mapPtr = std::make_unique<MemoryMap>();

    auto& typedMap = *mapPtr;
    Interval interval = boost::icl::interval<uint64_t>::right_open(base, base + memory->Size());

    for (const auto& [existingInterval, _] : typedMap.map)
    {
        if (boost::icl::intersects(existingInterval, interval))
        {
            throw std::runtime_error("Overlapping region for this memory type");
        }
    }

    typedMap.map.insert({interval, raw});
    typedMap.baseOffsets[raw] = base;
    ownedMemory.push_back(std::move(memory));
}

void MemoryController::RegisterObserver(Memory* target, std::unique_ptr<MemoryObserver> obs)
{
    observers[target].emplace_back(std::move(obs));
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