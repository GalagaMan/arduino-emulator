#pragma once
#include "instruction.hxx"
#include "decoder.hxx"






class OR : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rd;
    uint8_t Rr;

public:
    OR(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr);
};


class ORMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit ORMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};