#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <memory>
#include <typeindex>


class AVRCpu;
class MemoryController;

class AVRRegisterAccessBehavior
{
public:
    virtual ~AVRRegisterAccessBehavior() = default;

    [[nodiscard]] virtual uint8_t GetRegister(uint8_t index) const = 0;
    virtual void SetRegister(uint8_t index, uint8_t value) = 0;

    [[nodiscard]] virtual uint8_t GetSREG() const = 0;
    virtual void SetSREG(uint8_t value) = 0;

    [[nodiscard]] virtual uint8_t GetSPL() const = 0;
    virtual void SetSPL(uint8_t value) = 0;

    [[nodiscard]] virtual uint8_t GetSPH() const = 0;
    virtual void SetSPH(uint8_t value) = 0;

    virtual void WriteToSRAM(uint16_t address, std::vector<uint8_t> const& data) = 0;
    virtual std::vector<uint8_t> ReadFromSRAM(uint16_t address, uint64_t amount) = 0;
};

enum class AvrIORegister
{
    A,
    B,
    C,
    D
};

class AVRIOBehavior
{
public:
    virtual ~AVRIOBehavior() = default;

    [[nodiscard]]virtual uint8_t GetPIN(AvrIORegister port) const = 0;

    [[nodiscard]]virtual uint8_t GetDDR(AvrIORegister port) const = 0;
    virtual void SetDDR(AvrIORegister port, uint8_t value) = 0;

    [[nodiscard]] virtual uint8_t GetPORT(AvrIORegister port) const = 0;
    virtual void SetPORT(AvrIORegister port, uint8_t value) = 0;
};

class MemoryBackedAccessBehavior : public AVRRegisterAccessBehavior
{
private:
    uint64_t rAddr;
    uint64_t sregAddr;
    uint64_t splAddr;
    uint64_t sphAddr;

    std::type_index sramMemType;
    std::type_index flashMemType;
    
    MemoryController* memoryController;

public:
    MemoryBackedAccessBehavior(MemoryController* memoryController, std::type_index sramMem,
                               std::type_index flashMemType, uint64_t rAddress, uint64_t sregAddress,
                               uint64_t splAddress, uint64_t sphAddress);

    [[nodiscard]] virtual uint8_t GetRegister(uint8_t index) const override;
    void SetRegister(uint8_t index, uint8_t value) override;

    [[nodiscard]] virtual uint8_t GetSREG() const override;
    void SetSREG(uint8_t value) override;

    [[nodiscard]] virtual uint8_t GetSPL() const override;
    void SetSPL(uint8_t value) override;

    [[nodiscard]] virtual uint8_t GetSPH() const override;
    void SetSPH(uint8_t value) override;

    void WriteToSRAM(uint16_t address, std::vector<uint8_t> const& data) override;
    std::vector<uint8_t> ReadFromSRAM(uint16_t address, uint64_t amount) override;
};

class MemoryMappedIOBehavior : public AVRIOBehavior
{
private:
    MemoryController* memoryController;
    uint64_t ioBaseOffset{};

public:
    MemoryMappedIOBehavior(MemoryController* memoryController, uint64_t ioBaseOffset);

    [[nodiscard]] uint8_t GetPIN(AvrIORegister port) const override;

    [[nodiscard]] uint8_t GetDDR(AvrIORegister port) const override;
    void SetDDR(AvrIORegister port, uint8_t value) override;

    [[nodiscard]] uint8_t GetPORT(AvrIORegister port) const override;
    void SetPORT(AvrIORegister port, uint8_t value) override;
};

struct AVRInstructionContext
{
    AVRCpu* cpu = nullptr;
    AVRRegisterAccessBehavior* accessBehavior = nullptr;
    MemoryController* memoryController = nullptr;
    AVRIOBehavior* ioBehavior = nullptr;
};

class Instruction
{
private:
    uint64_t opcode;
    uint64_t size;
    uint64_t cycles;
    std::function<void()> translation;
    std::string mnemonic;

public:
    Instruction(uint64_t opcode, uint64_t size, uint64_t cycles, std::function<void()> translation,
                std::string mnemonic);

    [[nodiscard]] uint64_t Opcode() const;
    [[nodiscard]] uint64_t Size() const;
    [[nodiscard]] uint64_t Cycles() const;
    [[nodiscard]] std::string const& Mnemonic() const;

    virtual ~Instruction() = default;
    virtual void Execute() const;
};

class InstructionSet
{
private:
    std::unordered_map<uint64_t, std::shared_ptr<Instruction>> instructions;

public:
    void AddInstruction(std::shared_ptr<Instruction> instruction);

    [[nodiscard]] Instruction const& Instruction(uint64_t opcode) const;
    [[nodiscard]] bool HasInstruction(uint64_t opcode) const;
};