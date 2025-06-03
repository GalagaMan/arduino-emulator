#pragma once
#include "decoder.hxx"
#include "instruction.hxx"


class BREQ : public Instruction
{
private:
    AVRInstructionContext* ctx;
    int8_t offset;

public:
    BREQ(AVRInstructionContext* ctx, int8_t offset);
};

class BREQMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit BREQMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};