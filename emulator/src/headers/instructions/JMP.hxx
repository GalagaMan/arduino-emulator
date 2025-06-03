#pragma once
#include "decoder.hxx"
#include "instruction.hxx"

uint16_t EncodeJMPLow(uint32_t addr);
uint16_t EncodeJMPHigh(uint32_t addr);

class JMP : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint32_t addr;

public:
    JMP(AVRInstructionContext* ctx, uint32_t addr);
};

class JMPMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit JMPMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};
