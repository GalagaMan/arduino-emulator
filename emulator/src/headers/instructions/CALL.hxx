#pragma once
#include "decoder.hxx"
#include "instruction.hxx"


uint16_t EncodeCALLLow(uint16_t addr);
uint16_t EncodeCALLHigh(uint16_t addr);

class CALL : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint16_t addr;

public:
    CALL(AVRInstructionContext* ctx, uint16_t addr);
};

class CALLMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit CALLMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};