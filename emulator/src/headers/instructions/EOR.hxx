#pragma once
#include "instruction.hxx"
#include "decoder.hxx"
#include <cstdint>


class EOR : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rd;
    uint8_t Rr;

public:
    EOR(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr);
};


class EORMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit EORMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};