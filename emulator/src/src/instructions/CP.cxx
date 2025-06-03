#include "instructions/CP.hxx"
#include "cpu.hxx"
#include <format>

namespace
{
void CpImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr)
{
    uint8_t a = ctx->accessBehavior->GetRegister(Rd);
    uint8_t b = ctx->accessBehavior->GetRegister(Rr);
    uint8_t result = a - b;

    bool R = result & 0x80;
    bool H = (~a & b) & 0x08;
    bool V = ((a ^ b) & (a ^ result)) & 0x80;
    bool N = result & 0x80;
    bool S = N ^ V;
    bool Z = result == 0;
    bool C = a < b;

    uint8_t sreg = ctx->accessBehavior->GetSREG();

    sreg &= ~(1 << 0);
    if (C)
        sreg |= (1 << 0); // C
    sreg &= ~(1 << 1);
    if (Z)
        sreg |= (1 << 1); // Z
    sreg &= ~(1 << 2);
    if (N)
        sreg |= (1 << 2); // N
    sreg &= ~(1 << 3);
    if (V)
        sreg |= (1 << 3); // V
    sreg &= ~(1 << 4);
    if (S)
        sreg |= (1 << 4); // S
    sreg &= ~(1 << 5);
    if (H)
        sreg |= (1 << 5); // H

    ctx->accessBehavior->SetSREG(sreg);
    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

CP::CP(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr)
    : Instruction(
          0x1400, 2, 1, [=]() { CpImpl(ctx, Rd, Rr); }, std::format("CP R{}, R{}", Rd, Rr)),
      ctx(ctx), Rd(Rd), Rr(Rr)
{
}

CPMatcher::CPMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t CPMatcher::MinLength() const
{
    return 2;
}

bool CPMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xFC00) == 0x1400;
}

std::unique_ptr<Instruction> CPMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t Rd = (word >> 4) & 0x1F;
    uint8_t Rr = (word & 0x0F) | ((word >> 5) & 0x10);
    return std::make_unique<CP>(ctx, Rd, Rr);
}
