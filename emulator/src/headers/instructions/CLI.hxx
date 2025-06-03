#pragma once
#include "decoder.hxx"
#include "instruction.hxx"

uint16_t EncodeCLI();

class CLI : public Instruction
{
private:
    AVRInstructionContext* ctx;

public:
    explicit CLI(AVRInstructionContext* ctx);
};

class CLIMatcher : public DecodeRule
{
private:
    AVRInstructionContext* ctx;

public:
    explicit CLIMatcher(AVRInstructionContext* ctx);

    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
    [[nodiscard]] std::unique_ptr<Instruction> Decode(const std::vector<uint8_t>& bytes) const override;
};
