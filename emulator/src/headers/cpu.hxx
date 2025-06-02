#pragma once
#include <vector>
#include <functional>
#include <unordered_map>
#include <array>
#include <string>
#include <memory>


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

    void Execute() const;
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

class CPU
{
private:
    uint64_t speed{};
    std::shared_ptr<InstructionSet> instructionSet;
    Instruction* currentInstruction = nullptr;

public:
    CPU(uint64_t speed, std::shared_ptr<InstructionSet> instructionSet);

    [[nodiscard]] InstructionSet const& InstructionSet() const;
    [[nodiscard]] Instruction const& CurrentInstruction() const;

    
    virtual ~CPU() = default;
};

// class MemoryController;

class AVRCpu : public CPU
{
private:
    // MemoryController* memoryController;
    std::array<uint8_t, 32> registers;
    uint8_t sreg;
    uint16_t sp;
    uint16_t pc;

public:
    AVRCpu();

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