#include "instructions/PUSH.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <format>

namespace
{
void PushImpl(AVRInstructionContext* ctx, uint8_t Rr)
{
    uint8_t spLow = ctx->accessBehavior->GetSPL();
    uint8_t spHigh = ctx->accessBehavior->GetSPH();
    uint16_t sp = (static_cast<uint16_t>(spHigh) << 8) | spLow;

    --sp;
    ctx->accessBehavior->SetSPL(static_cast<uint8_t>(sp & 0xFF));
    ctx->accessBehavior->SetSPH(static_cast<uint8_t>(sp >> 8));

    uint8_t value = ctx->accessBehavior->GetRegister(Rr);
    ctx->accessBehavior->WriteToSRAM(sp, {value});

    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

PUSH::PUSH(AVRInstructionContext* ctx, uint8_t Rr)
    : Instruction(0x920F, 2, 2, [=]() { PushImpl(ctx, Rr); }, std::format("PUSH R{}", Rr)), ctx(ctx), Rr(Rr)
{
}

PUSHMatcher::PUSHMatcher(AVRInstructionContext* ctx) : ctx(ctx)
{
}

size_t PUSHMatcher::MinLength() const
{
    return 2;
}

bool PUSHMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xFE0F) == 0x920F;
}

std::unique_ptr<Instruction> PUSHMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t Rr = (word >> 4) & 0x1F;

    return std::make_unique<PUSH>(ctx, Rr);
}
