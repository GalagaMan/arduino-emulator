#include "instructions/CLR.hxx"
#include "cpu.hxx"
#include <format>



namespace
{
void ClrImpl(AVRInstructionContext* ctx, uint8_t Rd)
{
    ctx->accessBehavior->SetRegister(Rd, 0);

    uint8_t sreg = ctx->accessBehavior->GetSREG();
    sreg |= (1 << 1);  // Z = 1
    sreg &= ~(1 << 2); // N = 0
    sreg &= ~(1 << 3); // V = 0
    sreg &= ~(1 << 4); // S = 0
    ctx->accessBehavior->SetSREG(sreg);

    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

CLR::CLR(AVRInstructionContext* ctx, uint8_t Rd)
    : Instruction(
          0x2400 | (Rd << 4) | (Rd & 0x0F), 2, 1, [=]() { ClrImpl(ctx, Rd); }, std::format("CLR R{}", Rd)),
      ctx(ctx), Rd(Rd)
{
}

CLRMatcher::CLRMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{}

size_t CLRMatcher::MinLength() const
{
    return 2;
}

bool CLRMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;
    uint16_t word = bytes[0] | (bytes[1] << 8);

    if ((word & 0xFC00) != 0x2400)
        return false;

    uint8_t Rd = (word >> 4) & 0x1F;
    uint8_t Rr = (word & 0x0F) | ((word >> 5) & 0x10);
    return Rd == Rr;
}

std::unique_ptr<Instruction> CLRMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t Rd = (word >> 4) & 0x1F;
    return std::make_unique<CLR>(ctx, Rd);
}