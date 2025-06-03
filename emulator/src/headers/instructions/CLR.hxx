#pragma once
#include "instruction.hxx"
#include "decoder.hxx"



class CLR : public Instruction
{
private:
    AVRInstructionContext* ctx;
    uint8_t Rd;

public:
    CLR(AVRInstructionContext* ctx, uint8_t Rd);
};



class CLRMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit CLRMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};