#include "instructions/RJMP.hxx"
#include "cpu.hxx"
#include <format>





namespace
{
void RJMPImpl(AVRInstructionContext* ctx, int16_t offset)
{
    auto pc = ctx->cpu->GetPC();
    ctx->cpu->SetPC(static_cast<uint16_t>(pc + offset * 2));
}
} // namespace

RJMP::RJMP(AVRInstructionContext* ctx, int16_t offset)
    : Instruction(
          0xC000, 2, 2, [=]() { RJMPImpl(ctx, offset); }, std::format("RJMP {}", offset)),
      ctx(ctx), offset(offset)
{
}


RJMPMatcher::RJMPMatcher(AVRInstructionContext* ctx) : ctx(ctx)
{
}

size_t RJMPMatcher::MinLength() const
{
    return 2;
}

bool RJMPMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xF000) == 0xC000;
}

std::unique_ptr<Instruction> RJMPMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    int16_t offset = static_cast<int16_t>(word & 0x0FFF);
    if (offset & 0x0800) // sign extend 12-bit to 16-bit
        offset |= 0xF000;

    return std::make_unique<RJMP>(ctx, offset);
}
