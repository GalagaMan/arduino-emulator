#pragma once
#include "decoder.hxx"
#include "instruction.hxx"

class POP : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rr;

public:
    POP(AVRInstructionContext* ctx, uint8_t Rr);
};

class POPMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit POPMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};