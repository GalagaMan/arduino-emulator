#include "instructions/ORI.hxx"
#include "cpu.hxx"
#include <format>


namespace
{
void OriImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K)
{
    auto val = ctx->accessBehavior->GetRegister(Rd);
    auto result = static_cast<uint8_t>(val | K);
    ctx->accessBehavior->SetRegister(Rd, result);

    uint8_t sreg = ctx->accessBehavior->GetSREG();

    bool R = result & 0x80;
    bool V = false;
    bool N = result & 0x80;
    bool S = N ^ V;
    bool Z = result == 0;

    sreg &= ~(1 << 1); if (Z) sreg |= (1 << 1); // Z
    sreg &= ~(1 << 2); if (N) sreg |= (1 << 2); // N
    sreg &= ~(1 << 3); if (V) sreg |= (1 << 3); // V
    sreg &= ~(1 << 4); if (S) sreg |= (1 << 4); // S

    ctx->accessBehavior->SetSREG(sreg);
    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

ORI::ORI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K)
    : Instruction(
          0x6000 | ((K & 0xF0) << 4) | ((Rd - 16) << 4) | (K & 0x0F),
          2, 1, [=]() { OriImpl(ctx, Rd, K); },
          std::format("ORI R{}, 0x{:02X}", Rd, K)),
      ctx(ctx), Rd(Rd), K(K)
{
}

ORIMatcher::ORIMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t ORIMatcher::MinLength() const
{
    return 2;
}

bool ORIMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xF000) == 0x6000;
}

std::unique_ptr<Instruction> ORIMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);

    uint8_t K = ((word >> 4) & 0xF0) | (word & 0x0F);
    uint8_t Rd = 16 + ((word >> 4) & 0x0F);

    return std::make_unique<ORI>(ctx, Rd, K);
}