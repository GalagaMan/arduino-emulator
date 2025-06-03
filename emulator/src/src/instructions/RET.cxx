#include "instructions/RET.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <format>

uint16_t EncodeRET()
{
    return 0x9508;
}

namespace
{
void RetImpl(AVRInstructionContext* ctx)
{
    uint16_t sp = (ctx->accessBehavior->GetSPH() << 8) | ctx->accessBehavior->GetSPL();

    auto low = ctx->accessBehavior->ReadFromSRAM(sp, 1);
    auto high = ctx->accessBehavior->ReadFromSRAM(sp + 1, 1);

    uint16_t retAddr = low[0] | (high[0] << 8);
    ctx->cpu->SetPC(retAddr);

    sp += 2;
    ctx->accessBehavior->SetSPH(sp >> 8);
    ctx->accessBehavior->SetSPL(sp & 0xFF);
}
} // namespace

RET::RET(AVRInstructionContext* ctx)
    : Instruction(
          0x9508, 2, 4, [=]() { RetImpl(ctx); }, "RET"),
      ctx(ctx)
{
}

RETMatcher::RETMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t RETMatcher::MinLength() const
{
    return 2;
}

bool RETMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return word == 0x9508;
}

std::unique_ptr<Instruction> RETMatcher::Decode([[maybe_unused]] const std::vector<uint8_t>& bytes) const
{
    return std::make_unique<RET>(ctx);
}
