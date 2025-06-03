#pragma once
#include "instruction.hxx"
#include "decoder.hxx"

class RJMP : public Instruction
{
private:
    AVRInstructionContext* ctx;
    int16_t offset;

public:
    RJMP(AVRInstructionContext* ctx, int16_t offset);
};


class RJMPMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit RJMPMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};