#include "cpu.hxx"
#include "instructions/CRI.hxx"
#include <format>




namespace
{
void CPIImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K)
{
    uint8_t a = ctx->accessBehavior->GetRegister(Rd);
    uint8_t result = a - K;

    bool H = ((~a & K) | (K & result) | (result & ~a)) & 0x08;
    bool V = ((a ^ K) & (a ^ result)) & 0x80;
    bool N = result & 0x80;
    bool S = N ^ V;
    bool Z = result == 0;
    bool C = K > a;

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

CPI::CPI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t K)
    : Instruction(
          0x3000, 2, 1, [=]() { CPIImpl(ctx, Rd, K); }, std::format("CPI R{}, {}", Rd, K)),
      ctx(ctx), Rd(Rd), K(K)
{
}

CPIMatcher::CPIMatcher(AVRInstructionContext* ctx) : ctx(ctx)
{
}

size_t CPIMatcher::MinLength() const
{
    return 2;
}

bool CPIMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xF000) == 0x3000;
}

std::unique_ptr<Instruction> CPIMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t K = ((word >> 4) & 0xF0) | (word & 0x0F);
    uint8_t Rd = 16 + ((word >> 8) & 0x0F);
    return std::make_unique<CPI>(ctx, Rd, K);
}
