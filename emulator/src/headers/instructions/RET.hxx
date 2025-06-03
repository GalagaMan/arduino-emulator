#pragma once
#include "decoder.hxx"
#include "instruction.hxx"

uint16_t EncodeRET();

class RET : public Instruction
{
private:
    AVRInstructionContext* ctx;

public:
    explicit RET(AVRInstructionContext* ctx);
};

class RETMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit RETMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};
