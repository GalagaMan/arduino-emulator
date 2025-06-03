#pragma once
#include "decoder.hxx"
#include "instruction.hxx"


class CP : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rd;
    uint8_t Rr;

public:
    CP(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr);
};

class CPMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit CPMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};