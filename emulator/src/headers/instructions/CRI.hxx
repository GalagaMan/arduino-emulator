#pragma once
#include "instruction.hxx"
#include "decoder.hxx"

class CPI : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rd;
    uint8_t K;

public:
    CPI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K);
};

class CPIMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit CPIMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};