#include "instructions/SBCI.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <format>

uint16_t EncodeSBCI(uint8_t Rd, uint8_t K)
{
    return 0x4000 | ((Rd - 16) << 4) | (K & 0x0F) | ((K & 0xF0) << 4);
}

namespace
{
void SbciImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K)
{
    uint8_t RdVal = ctx->accessBehavior->GetRegister(Rd);
    uint8_t sreg = ctx->accessBehavior->GetSREG();
    bool carryIn = sreg & (1 << 0);

    uint16_t result = static_cast<uint16_t>(RdVal) - K - carryIn;
    uint8_t res8 = static_cast<uint8_t>(result);

    ctx->accessBehavior->SetRegister(Rd, res8);

    // Flags
    bool R = res8;
    bool N = res8 & 0x80;
    bool Z = res8 == 0 && !(sreg & (1 << 1)); // Z is cleared only if previous Z was clear
    bool V = ((RdVal ^ K) & (RdVal ^ res8)) & 0x80;
    bool C = result > 0xFF;
    bool S = N ^ V;
    bool H = (~RdVal & K) | (K & res8) | (res8 & ~RdVal);

    sreg &= 0xC0; // Clear all except I and T
    if (C)
        sreg |= (1 << 0);
    if (Z)
        sreg |= (1 << 1);
    if (N)
        sreg |= (1 << 2);
    if (V)
        sreg |= (1 << 3);
    if (S)
        sreg |= (1 << 4);
    if (H)
        sreg |= (1 << 5);

    ctx->accessBehavior->SetSREG(sreg);
    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

SBCI::SBCI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K)
    : Instruction(
          0x4000, 2, 1, [=]() { SbciImpl(ctx, Rd, K); }, std::format("SBCI R{}, 0x{:02X}", Rd, K)),
      ctx(ctx), Rd(Rd), K(K)
{
}

SBCIMatcher::SBCIMatcher(AVRInstructionContext* ctx) : ctx(ctx)
{
}

size_t SBCIMatcher::MinLength() const
{
    return 2;
}

bool SBCIMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xF000) == 0x4000;
}

std::unique_ptr<Instruction> SBCIMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t K = (word & 0x0F) | ((word >> 4) & 0xF0);
    uint8_t Rd = ((word >> 4) & 0xF) + 16;
    return std::make_unique<SBCI>(ctx, Rd, K);
}
