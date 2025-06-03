#include "instructions/SUB.hxx"
#include "cpu.hxx"
#include "memory.hxx"


namespace
{
void SubImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr)
{
    uint8_t a = ctx->accessBehavior->GetRegister(Rd);
    uint8_t b = ctx->accessBehavior->GetRegister(Rr);
    uint8_t result = a - b;

    ctx->accessBehavior->SetRegister(Rd, result);

    uint8_t sreg = ctx->accessBehavior->GetSREG();

    bool Rd3 = a & (1 << 3);
    bool Rr3 = b & (1 << 3);
    bool R3 = result & (1 << 3);

    bool Rd7 = a & (1 << 7);
    bool Rr7 = b & (1 << 7);
    bool R7 = result & (1 << 7);

    bool H = (!Rd3 & Rr3) | (Rr3 & R3) | (R3 & !Rd3);
    bool V = (Rd7 & !Rr7 & !R7) | (!Rd7 & Rr7 & R7);
    bool N = R7;
    bool S = N ^ V;
    bool Z = result == 0;
    bool C = (!Rd7 & Rr7) | (Rr7 & R7) | (R7 & !Rd7);

    // Write back flags
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

SUB::SUB(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr)
    : Instruction(
          0x1800, 2, 1, [=]() { SubImpl(ctx, Rd, Rr); }, std::format("SUB R{}, R{}", Rd, Rr)),
      ctx(ctx), Rd(Rd), Rr(Rr)
{
}


SUBMatcher::SUBMatcher(AVRInstructionContext* ctx) : ctx(ctx)
{
}

size_t SUBMatcher::MinLength() const
{
    return 2;
}

bool SUBMatcher::Matches(std::vector<uint8_t> const& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xFC00) == 0x1800;
}

std::unique_ptr<Instruction> SUBMatcher::Decode(std::vector<uint8_t> const& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t Rd = (word >> 4) & 0x1F;
    uint8_t Rr = (word & 0x0F) | ((word >> 5) & 0x10);
    return std::make_unique<SUB>(ctx, Rd, Rr);
}