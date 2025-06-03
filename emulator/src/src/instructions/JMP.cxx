#include "instructions/JMP.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <format>

uint16_t EncodeJMPLow(uint32_t addr)
{
    return addr & 0xFFFF;
}

uint16_t EncodeJMPHigh(uint32_t addr)
{
    return 0x940C | ((addr >> 16) & 0x01F0);
}

namespace
{
void JmpImpl(AVRInstructionContext* ctx, uint32_t addr)
{
    ctx->cpu->SetPC(addr);
}
} // namespace

JMP::JMP(AVRInstructionContext* ctx, uint32_t addr)
    : Instruction(0x940C, 4, 4, [=]() { JmpImpl(ctx, addr); }, std::format("JMP 0x{:06X}", addr)), ctx(ctx), addr(addr)
{
}

JMPMatcher::JMPMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t JMPMatcher::MinLength() const
{
    return 4;
}

bool JMPMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 4)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xFE0E) == 0x940C;
}

std::unique_ptr<Instruction> JMPMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t high = bytes[0] | (bytes[1] << 8);
    uint16_t low = bytes[2] | (bytes[3] << 8);
    uint32_t addr = ((high & 0x01F0) << 13) | low;
    return std::make_unique<JMP>(ctx, addr);
}
