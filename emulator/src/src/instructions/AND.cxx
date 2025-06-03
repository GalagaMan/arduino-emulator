#include "instructions/AND.hxx"
#include "cpu.hxx"
#include "decoder.hxx"
#include "memory.hxx"


namespace
{
void AndImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr)
{
    auto a = ctx->accessBehavior->GetRegister(Rd);
    auto b = ctx->accessBehavior->GetRegister(Rr);
    uint8_t res = a & b;

    ctx->accessBehavior->SetRegister(Rd, res);

    uint8_t sreg = ctx->accessBehavior->GetSREG();

    bool N = res & 0x80;
    bool Z = res == 0;
    bool V = false;
    bool S = N ^ V;

    sreg &= ~(1 << 1);
    if (Z)
        sreg |= (1 << 1); // Z
    sreg &= ~(1 << 2);
    if (N)
        sreg |= (1 << 2); // N
    sreg &= ~(1 << 3);    // V always cleared
    sreg &= ~(1 << 4);
    if (S)
        sreg |= (1 << 4); // S

    ctx->accessBehavior->SetSREG(sreg);
    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

AND::AND(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr)
    : Instruction(
          0x2000, 2, 1, [=]() { AndImpl(ctx, Rd, Rr); }, std::format("AND R{}, R{}", Rd, Rr)),
      ctx(ctx), Rd(Rd), Rr(Rr)
{
}


ANDMatcher::ANDMatcher(AVRInstructionContext* ctx) : ctx(ctx)
{
}

size_t ANDMatcher::MinLength() const
{
    return 2;
}

bool ANDMatcher::Matches(std::vector<uint8_t> const& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xFC00) == 0x2000;
}

std::unique_ptr<Instruction> ANDMatcher::Decode(std::vector<uint8_t> const& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t Rd = (word >> 4) & 0x1F;
    uint8_t Rr = (word & 0x0F) | ((word >> 5) & 0x10);
    return std::make_unique<AND>(ctx, Rd, Rr);
}