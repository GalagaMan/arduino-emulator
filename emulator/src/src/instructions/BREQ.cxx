#include "instructions/BREQ.hxx"
#include "cpu.hxx"
#include <format>

namespace
{
void BreqImpl(AVRInstructionContext* ctx, int8_t offset)
{
    auto sreg = ctx->accessBehavior->GetSREG();
    bool zero = sreg & (1 << 1);
    auto pc = ctx->cpu->GetPC();

    if (zero)
        ctx->cpu->SetPC(static_cast<uint16_t>(pc + offset * 2));
    else
        ctx->cpu->SetPC(pc + 2);
}
} // namespace

BREQ::BREQ(AVRInstructionContext* ctx, int8_t offset)
    : Instruction(
          0xF001, 2, 1, [=]() { BreqImpl(ctx, offset); }, std::format("BREQ {}", offset)),
      ctx(ctx), offset(offset)
{
}

BREQMatcher::BREQMatcher(AVRInstructionContext* ctx) : ctx(ctx)
{
}

size_t BREQMatcher::MinLength() const
{
    return 2;
}

bool BREQMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xFE07) == 0xF001;
}

std::unique_ptr<Instruction> BREQMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    int8_t offset = static_cast<int8_t>((word << 3)) >> 3;
    return std::make_unique<BREQ>(ctx, offset);
}
