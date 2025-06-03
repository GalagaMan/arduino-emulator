#include "instructions/ANDI.hxx"
#include "cpu.hxx"
#include <format>

namespace
{
void AndiImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K)
{
    auto val = ctx->accessBehavior->GetRegister(Rd);
    uint8_t result = val & K;
    ctx->accessBehavior->SetRegister(Rd, result);

    uint8_t sreg = ctx->accessBehavior->GetSREG();

    bool R = result & 0x80;
    bool N = result & 0x80;
    bool Z = result == 0;
    bool V = false;
    bool S = N ^ V;

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

    ctx->accessBehavior->SetSREG(sreg);
    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

ANDI::ANDI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K)
    : Instruction(
          0x7000, 2, 1, [=]() { AndiImpl(ctx, Rd, K); }, std::format("ANDI R{}, 0x{:02X}", Rd, K)),
      ctx(ctx), Rd(Rd), K(K)
{
}

ANDIMatcher::ANDIMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t ANDIMatcher::MinLength() const
{
    return 2;
}

bool ANDIMatcher::Matches(std::vector<uint8_t> const& bytes) const
{
    if (bytes.size() < 2)
        return false;
    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xF000) == 0x7000;
}

std::unique_ptr<Instruction> ANDIMatcher::Decode(std::vector<uint8_t> const& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t K = ((word >> 8) & 0x0F) << 4 | (word & 0x0F);
    uint8_t Rd = 16 + ((word >> 4) & 0x0F);
    return std::make_unique<ANDI>(ctx, Rd, K);
}
