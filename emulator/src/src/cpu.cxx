#include "cpu.hxx"
#include <stdexcept>
#include <format>
#include <cassert>




Instruction::Instruction(uint64_t opcode, uint64_t size, uint64_t cycles, std::function<void()> translation,
                         std::string mnemonic)
    : opcode(opcode), size(size), cycles(cycles), translation(std::move(translation)), mnemonic(std::move(mnemonic))
{
    if (!translation)
        throw std::invalid_argument("Translation function cannot be empty");
}

uint64_t Instruction::Opcode() const
{
    return opcode;
}

uint64_t Instruction::Size() const
{
    return size;
}

uint64_t Instruction::Cycles() const
{
    return cycles;
}

std::string const& Instruction::Mnemonic() const
{
    return mnemonic;
}

void Instruction::Execute() const
{
    translation();
}

void InstructionSet::AddInstruction(std::shared_ptr<class Instruction> instruction)
{
    if (!instruction)
        throw std::invalid_argument("Instruction cannot be empty function");

    if (instructions.contains(instruction->Opcode()))
        throw std::runtime_error(std::format("Instruction with opcode {} already exists", instruction->Opcode()));

    instructions[instruction->Opcode()] = std::move(instruction);
}

bool InstructionSet::HasInstruction(uint64_t opcode) const
{
    return instructions.contains(opcode);
}

Instruction const& InstructionSet::Instruction(uint64_t opcode) const
{
    if (!HasInstruction(opcode))
        throw std::runtime_error(std::format("Instruction with opcode {} not found", opcode));

    return *instructions.at(opcode);
}

CPU::CPU(uint64_t speed, std::shared_ptr<class InstructionSet> instructionSet)
    : speed(speed), instructionSet(std::move(instructionSet))
{
    if (speed == 0)
        throw std::invalid_argument("CPU speed cannot be zero");

    if (!this->instructionSet)
        throw std::invalid_argument("InstructionSet cannot be empty");
}

Instruction const& CPU::CurrentInstruction() const
{
    assert(instructionSet);

    return *currentInstruction;
}


InstructionSet const& CPU::InstructionSet() const
{
    return *instructionSet;
}

