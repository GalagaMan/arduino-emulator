#pragma once
#include "instruction.hxx"
#include "decoder.hxx"

class BRNE : public Instruction
{
private:
    AVRInstructionContext* ctx;
    int8_t offset;

public:
    BRNE(AVRInstructionContext* ctx, int8_t offset);
};

class BRNEMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit BRNEMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};
