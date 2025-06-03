#pragma once
#include "instruction.hxx"
#include "decoder.hxx"


class ANDI : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rd;
    uint8_t K;

public:
    ANDI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K);
};

class ANDIMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit ANDIMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};