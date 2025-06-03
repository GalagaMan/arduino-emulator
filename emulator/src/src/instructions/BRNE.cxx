#include "instructions/BRNE.hxx"
#include "cpu.hxx"
#include <format>

namespace
{
void BrneImpl(AVRInstructionContext* ctx, int8_t offset)
{
    auto sreg = ctx->accessBehavior->GetSREG();
    bool zero = sreg & (1 << 1);

    auto pc = ctx->cpu->GetPC();

    if (!zero)
        ctx->cpu->SetPC(static_cast<uint16_t>(pc + offset * 2));
    else
        ctx->cpu->SetPC(pc + 2);
}
} // namespace

BRNE::BRNE(AVRInstructionContext* ctx, int8_t offset)
    : Instruction(
          0xF401, 2, 1, [=]() { BrneImpl(ctx, offset); }, std::format("BRNE {}", static_cast<int>(offset))),
      ctx(ctx), offset(offset)
{
}

BRNEMatcher::BRNEMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t BRNEMatcher::MinLength() const
{
    return 2;
}

bool BRNEMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xFC07) == 0xF401;
}

std::unique_ptr<Instruction> BRNEMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    int8_t offset = static_cast<int8_t>((word << 3)) >> 3;

    return std::make_unique<BRNE>(ctx, offset);
}
