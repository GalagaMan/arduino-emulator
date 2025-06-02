#include <stdexcept>
#include "decoder.hxx"
#include "instruction.hxx"





void InstructionDecoder::AddRule(std::unique_ptr<DecodeRule> rule)
{
    if (!rule)
        throw std::runtime_error("Rule cannot be null");

    rules[rule->MinLength()].emplace_back(std::move(rule));
}

std::vector<std::unique_ptr<DecodeRule>> const* InstructionDecoder::InstructionsBySize(uint64_t size) const
{
    auto it = rules.find(size);

    if (it == rules.end())
        return nullptr;

    return &it->second;
}

std::unique_ptr<Instruction> AVRInstructionDecoder::Decode(std::vector<uint8_t> instructionBytes)
{
    auto const* decodeRules = InstructionsBySize(instructionBytes.size());

    if (!decodeRules)
        return nullptr;

    for (auto const& rule : *decodeRules)
    {
        if (rule->Matches(instructionBytes))
            return rule->Decode(instructionBytes);
    }

    return nullptr;
}