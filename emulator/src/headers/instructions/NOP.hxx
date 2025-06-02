#pragma once
#include "decoder.hxx"
#include "instruction.hxx"



class AVRCpu;
class MemoryController;

class NOP : public Instruction
{

};

class NOPMatcher : public DecodeRule
{
private:
    AVRCpu* cpu;
    MemoryController* memoryController;

public:
    [[nodiscard]] size_t MinLength() const override;
    [[nodiscard]] bool Matches(const std::vector<uint8_t>& bytes) const override;
};
