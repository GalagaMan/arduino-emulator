#pragma once
#include "decoder.hxx"
#include "instruction.hxx"


class PUSH : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rr;

public:
    PUSH(AVRInstructionContext* ctx, uint8_t Rr);
};

class PUSHMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit PUSHMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};
