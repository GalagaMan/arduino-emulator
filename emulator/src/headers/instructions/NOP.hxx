#pragma once
#include "decoder.hxx"
#include "instruction.hxx"



class AVRCpu;
class MemoryController;

class NOP : public Instruction
{
private:
    AVRInstructionContext* ctx;
public:
    NOP(AVRInstructionContext* ctx);
};

class NOPMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    NOPMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};
