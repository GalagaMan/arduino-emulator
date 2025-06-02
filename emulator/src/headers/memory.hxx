#pragma once
#include <boost/icl/interval_map.hpp>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>


class Memory
{
public:
    virtual ~Memory() = default;
    virtual void WriteLocation(uint64_t address, std::vector<unsigned char> const& data) = 0;
    [[nodiscard]] virtual std::vector<unsigned char> ReadLocation(uint64_t address, size_t size) const = 0;
    [[nodiscard]] virtual size_t Size() const = 0;
};

class MemoryObserver
{
public:
    virtual ~MemoryObserver() = default;
    virtual void NotifyWriteRequested(Memory* memory, uint64_t address, std::vector<unsigned char> const& data);
    virtual void NotifyWriteCompleted(Memory* memory, uint64_t address);

    virtual void NotifyReadRequested(Memory* memory, uint64_t address, size_t size);
    virtual void NotifyReadCompleted(Memory* memory, uint64_t address, std::vector<unsigned char> const& data);
};

class SRAMMemory : public Memory
{
    std::vector<unsigned char> storage_;

public:
    explicit SRAMMemory(size_t size);

    void WriteLocation(uint64_t address, std::vector<unsigned char> const& data) override;
    [[nodiscard]] std::vector<unsigned char> ReadLocation(uint64_t address, size_t size) const override;
    [[nodiscard]] size_t Size() const override;
};

class FlashMemory : public Memory
{
    std::vector<unsigned char> storage_;

public:
    explicit FlashMemory(size_t size);

    void WriteLocation(uint64_t address, std::vector<unsigned char> const& data) override;
    [[nodiscard]] std::vector<unsigned char> ReadLocation(uint64_t address, size_t size) const override;
    [[nodiscard]] size_t Size() const override;
};

class MemoryController
{
private:
    std::unordered_map<std::type_index, std::vector<std::unique_ptr<Memory>>> memoryUnits;
    boost::icl::interval_map<uint64_t, Memory*, boost::icl::total_absorber, std::less, boost::icl::inplace_identity> memoryMap;
    std::unordered_map<Memory*, std::vector<std::unique_ptr<MemoryObserver>>> memoryObservers;

public:
    virtual ~MemoryController() = default;

    void RegisterMemory(std::unique_ptr<Memory> memory, uint64_t start);

    template <typename T>
    void RegisterObserver(std::unique_ptr<MemoryObserver> observer, uint64_t startAddress, uint64_t endAddress)
    {
        if (startAddress > endAddress)
            throw std::invalid_argument("Start address must be less than or equal to end address");

        auto& memoryUnitsOfGivenType = memoryUnits[typeid(T)];

        if (memoryUnitsOfGivenType.empty())
            throw std::runtime_error("No memory units of the given type are registered");

        auto memory = memoryMap.find(boost::icl::interval<uint64_t>::left_open(startAddress, endAddress));

        if (memory == memoryMap.end())
            throw std::runtime_error("No memory unit found in the specified address range");

        memoryObservers[memory->second].emplace_back(std::move(observer));
    }

    template <typename MemoryType> std::vector<unsigned char> Read(uint64_t address, size_t size)
    {
        auto memoryEntry = memoryMap.find(address);
        if (memoryEntry == memoryMap.end())
            throw std::runtime_error("No memory unit found in the specified address range");

        if (typeid(MemoryType) != typeid(*memoryEntry->second))
            throw std::runtime_error("Memory type mismatch");

        const auto& interval = memoryEntry->first;
        uint64_t offset = address - interval.lower(); // Convert to local address

        for (auto& observer : memoryObservers[memoryEntry->second])
            observer->NotifyReadRequested(memoryEntry->second, address, size);

        auto data = memoryEntry->second->ReadLocation(offset, size);

        for (auto& observer : memoryObservers[memoryEntry->second])
            observer->NotifyReadCompleted(memoryEntry->second, address, data);

        return data;
    }

    template <typename MemoryType> void Write(uint64_t address, const std::vector<unsigned char>& data)
    {
        auto memoryEntry = memoryMap.find(address);
        if (memoryEntry == memoryMap.end())
            throw std::runtime_error("No memory unit found in the specified address range");

        if (typeid(MemoryType) != typeid(*memoryEntry->second))
            throw std::runtime_error("Memory type mismatch");

        const auto& interval = memoryEntry->first;
        uint64_t offset = address - interval.lower(); // Convert to local address

        for (auto& observer : memoryObservers[memoryEntry->second])
            observer->NotifyWriteRequested(memoryEntry->second, address, data);

        memoryEntry->second->WriteLocation(offset, data);

        for (auto& observer : memoryObservers[memoryEntry->second])
            observer->NotifyWriteCompleted(memoryEntry->second, address);
    }

    template <typename MemoryType> Memory* GetMemoryUnit(uint64_t address)
    {
        auto memory = memoryMap.find(address);
        if (memory == memoryMap.end())
            throw std::runtime_error("No memory unit found in the specified address range");

        if (typeid(MemoryType) != typeid(*memory->second))
            throw std::runtime_error("Memory type mismatch");

        return memory->second;
    }
};
