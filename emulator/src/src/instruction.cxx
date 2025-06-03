#include "instruction.hxx"
#include "memory.hxx"
#include <stdexcept>
#include <format>
#include <string>


MemoryBackedAccessBehavior::MemoryBackedAccessBehavior(MemoryController* memoryController, std::type_index sramMem,
                                                       std::type_index flashMemType, uint64_t rAddress,
                                                       uint64_t sregAddress, uint64_t splAddress, uint64_t sphAddress)
    : memoryController(memoryController), rAddr(rAddress), sregAddr(sregAddress), splAddr(splAddress), sphAddr(sphAddress), sramMemType(sramMem),
      flashMemType(flashMemType)
{
    if (!this->memoryController)
        throw std::runtime_error("Memory controller cannot be null");
}

uint8_t MemoryBackedAccessBehavior::GetRegister(uint8_t index) const
{
    if (index >= 32)
        throw std::out_of_range("Invalid AVR register index");

    auto result = memoryController->Read(sramMemType, rAddr + index, 1);
    return result.at(0);
}

void MemoryBackedAccessBehavior::SetRegister(uint8_t index, uint8_t value)
{
    if (index >= 32)
        throw std::out_of_range("Invalid AVR register index");

    memoryController->Write(sramMemType, rAddr + index, {value});
}

uint8_t MemoryBackedAccessBehavior::GetSREG() const
{
    auto result = memoryController->Read(sramMemType, sregAddr, 1);
    return result.at(0);
}

void MemoryBackedAccessBehavior::SetSREG(uint8_t value)
{
    memoryController->Write(sramMemType, sregAddr, { value });
}

uint8_t MemoryBackedAccessBehavior::GetSPL() const
{
    auto result = memoryController->Read(sramMemType, splAddr, 1);
    return result.at(0);
}

void MemoryBackedAccessBehavior::SetSPL(uint8_t value)
{
    memoryController->Write(sramMemType, splAddr, {value});
}

uint8_t MemoryBackedAccessBehavior::GetSPH() const
{
    auto result = memoryController->Read(sramMemType, sphAddr, 1);
    return result.at(0);
}

void MemoryBackedAccessBehavior::SetSPH(uint8_t value)
{
    memoryController->Write(sramMemType, sphAddr, {value});
}


Instruction::Instruction(uint64_t opcode, uint64_t size, uint64_t cycles, std::function<void()> translation,
                         std::string mnemonic)
    : opcode(opcode), size(size), cycles(cycles), translation(std::move(translation)), mnemonic(std::move(mnemonic))
{
    if (!this->translation)
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