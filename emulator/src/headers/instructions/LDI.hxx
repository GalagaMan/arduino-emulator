#pragma once
#include "instruction.hxx"
#include "decoder.hxx"
#include <cstdint>





class LDI : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rd;
    uint8_t imm;

public:
    LDI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t imm);
};


class LDIMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit LDIMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};
