#include "instructions/ADD.hxx"
#include "cpu.hxx"
#include <format>



namespace
{
void AddImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr)
{
    auto a = ctx->accessBehavior->GetRegister(Rd);
    auto b = ctx->accessBehavior->GetRegister(Rr);
    uint16_t result = static_cast<uint16_t>(a) + static_cast<uint16_t>(b);
    uint8_t res8 = static_cast<uint8_t>(result);

    ctx->accessBehavior->SetRegister(Rd, res8);

    uint8_t sreg = ctx->accessBehavior->GetSREG();

    bool R = res8 & 0x80;
    bool H = ((a & 0x0F) + (b & 0x0F)) & 0x10;
    bool V = ((~(a ^ b)) & (a ^ res8)) & 0x80;
    bool N = res8 & 0x80;
    bool S = N ^ V;
    bool Z = res8 == 0;
    bool C = result > 0xFF;

    sreg &= ~(1 << 0); if (C) sreg |= (1 << 0);      // C
    sreg &= ~(1 << 1); if (Z) sreg |= (1 << 1);      // Z
    sreg &= ~(1 << 2); if (N) sreg |= (1 << 2);      // N
    sreg &= ~(1 << 3); if (V) sreg |= (1 << 3);      // V
    sreg &= ~(1 << 4); if (S) sreg |= (1 << 4);      // S
    sreg &= ~(1 << 5); if (H) sreg |= (1 << 5);      // H

    ctx->accessBehavior->SetSREG(sreg);
    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

ADD::ADD(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr)
    : Instruction(0x0C00, 2, 1, [=]() { AddImpl(ctx, Rd, Rr); }, std::format("ADD R{}, R{}", Rd, Rr)),
      ctx(ctx), Rd(Rd), Rr(Rr)
{
}

ADDMatcher::ADDMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t ADDMatcher::MinLength() const
{
    return 2;
}

bool ADDMatcher::Matches(std::vector<uint8_t> const& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xFC00) == 0x0C00;
}

std::unique_ptr<Instruction> ADDMatcher::Decode(std::vector<uint8_t> const& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);

    uint8_t Rd = (word >> 4) & 0x1F;
    uint8_t Rr = (word & 0x0F) | ((word >> 5) & 0x10);

    return std::make_unique<ADD>(ctx, Rd, Rr);
}