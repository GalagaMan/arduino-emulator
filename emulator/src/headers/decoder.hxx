#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <functional>


class CPU;
class MemoryController;
class Instruction;


class DecodeRule
{
public:
    virtual ~DecodeRule() = default;

    [[nodiscard]] virtual bool Matches(const std::vector<uint8_t>& bytes) const = 0;
    [[nodiscard]] virtual size_t MinLength() const = 0;
    [[nodiscard]] virtual std::unique_ptr<Instruction> Decode(std::vector<uint8_t> const& bytes) const = 0;
};

class InstructionDecoder
{
private:
    std::unordered_map<uint64_t, std::vector<std::unique_ptr<DecodeRule>>> rules;

public:
    virtual ~InstructionDecoder() = default;
    virtual std::unique_ptr<Instruction> Decode(std::vector<uint8_t> instructionBytes) = 0;

    void AddRule(std::unique_ptr<DecodeRule> rule);
    [[nodiscard]] std::vector<std::unique_ptr<DecodeRule>> const* InstructionsBySize(uint64_t size) const;
};

class AVRInstructionDecoder : public InstructionDecoder
{
public:
    std::unique_ptr<Instruction> Decode(std::vector<uint8_t> instructionBytes) override;
};