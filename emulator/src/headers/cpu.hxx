#pragma once
#include <vector>
#include <functional>
#include <unordered_map>
#include <array>
#include <string>
#include <memory>
#include "instruction.hxx"


class CPU
{
private:
    uint64_t speed{};
    MemoryController* memoryController;

protected:
    [[nodiscard]] MemoryController* const MemoryController() const;

public:
    CPU(uint64_t speed, class MemoryController* memoryController);
    
    virtual ~CPU() = default;
};

class AVRCpu : public CPU
{
private:
    std::array<uint8_t, 32> registers;
    uint8_t sreg;
    uint64_t sp;
    uint64_t pc;

public:
    AVRCpu(uint64_t speed, class MemoryController* memoryController);

    uint8_t GetRegister(uint8_t index) const;
    void SetRegister(uint8_t index, uint8_t value);

    uint8_t GetSREG() const;
    void SetSREG(uint8_t value);

    bool GetFlagC() const;
    void SetFlagC(bool val);

    uint16_t GetSP() const;
    void SetSP(uint16_t value);

    uint8_t GetSPL() const;
    void SetSPL(uint8_t value);

    uint8_t GetSPH() const;
    void SetSPH(uint8_t value);

    uint16_t GetPC() const;
    void SetPC(uint16_t value);
};