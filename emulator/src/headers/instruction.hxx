#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <memory>



// struct

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