#pragma once
#include "instruction.hxx"
#include "decoder.hxx"


class SUBI : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rd;
    uint8_t imm;

public:
    SUBI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t imm);
};


class SUBIMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit SUBIMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};