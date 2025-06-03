#pragma once
#include "instruction.hxx"
#include "decoder.hxx"





class ORI : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rd;
    uint8_t K;

public:
    ORI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K);
};

class ORIMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit ORIMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};