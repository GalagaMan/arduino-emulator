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
    using Interval = boost::icl::interval<uint64_t>::type;

    struct MemoryMap
    {
        boost::icl::interval_map<uint64_t, Memory*> map;
        std::unordered_map<Memory*, uint64_t> baseOffsets;
    };

    std::unordered_map<std::type_index, std::unique_ptr<MemoryMap>> memoryMaps;
    std::unordered_map<Memory*, std::vector<std::unique_ptr<MemoryObserver>>> observers;
    std::vector<std::unique_ptr<Memory>> ownedMemory;


public:
    template <typename T>
    std::vector<uint8_t> Read(uint64_t address, size_t size)
    {
        auto it = memoryMaps.find(typeid(T));
        if (it == memoryMaps.end())
            throw std::runtime_error("Memory type not registered");

        auto& typedMap = *it->second;
        auto mapIt = typedMap.map.find(address);
        if (mapIt == typedMap.map.end())
            throw std::runtime_error("Address not mapped");

        Memory* mem = mapIt->second;
        uint64_t offset = address - typedMap.baseOffsets.at(mem);

        for (auto& obs : observers[mem])
            obs->NotifyReadRequested(mem, address, size);

        auto result = mem->ReadLocation(offset, size);

        for (auto& obs : observers[mem])
            obs->NotifyReadCompleted(mem, address, result);

        return result;
    }

    template <typename T>
    void Write(uint64_t address, const std::vector<uint8_t>& data)
    {
        auto it = memoryMaps.find(typeid(T));
        if (it == memoryMaps.end())
            throw std::runtime_error("Memory type not registered");

        auto& typedMap = *it->second;
        auto mapIt = typedMap.map.find(address);
        if (mapIt == typedMap.map.end())
            throw std::runtime_error("Address not mapped");

        Memory* mem = mapIt->second;
        uint64_t offset = address - typedMap.baseOffsets.at(mem);

        for (auto& obs : observers[mem])
            obs->NotifyWriteRequested(mem, address, data);

        mem->WriteLocation(offset, data);

        for (auto& obs : observers[mem])
            obs->NotifyWriteCompleted(mem, address);
    }

    template <typename T>
    void RegisterMemoryAs(std::unique_ptr<Memory> memory, uint64_t base, uint64_t size)
    {
        if (!memory)
            throw std::invalid_argument("Null memory");

        Memory* raw = memory.get();

        auto& typedMap = memoryMaps[typeid(T)];
        if (!typedMap)
            typedMap = std::make_unique<MemoryMap>();

        Interval interval = boost::icl::interval<uint64_t>::right_open(base, base + size);

        for (const auto& [existingInterval, _] : typedMap->map)
        {
            if (boost::icl::intersects(existingInterval, interval))
            {
                throw std::runtime_error("Overlapping region for this memory type");
            }
        }

        typedMap->map.insert({interval, raw});
        typedMap->baseOffsets[raw] = base;
        ownedMemory.push_back(std::move(memory));
    }

    std::vector<uint8_t> Read(std::type_index type, uint64_t address, size_t size);

    void Write(std::type_index type, uint64_t address, const std::vector<uint8_t>& data);

    void RegisterMemory(std::unique_ptr<Memory> memory, uint64_t base);

    void RegisterObserver(Memory* target, std::unique_ptr<MemoryObserver> obs);
};