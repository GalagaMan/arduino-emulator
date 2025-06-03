#include "instructions/LDI.hxx"
#include "cpu.hxx"
#include <format>


namespace
{
void LdiImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t imm)
{
    ctx->accessBehavior->SetRegister(Rd, imm);

    // update Z flag
    uint8_t sreg = ctx->accessBehavior->GetSREG();
    if (imm == 0)
        sreg |= (1 << 1); // Z
    else
        sreg &= ~(1 << 1);
    ctx->accessBehavior->SetSREG(sreg);

    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

LDI::LDI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t imm)
    : Instruction(
          0xE000 | ((imm & 0xF0) << 4) | ((Rd - 16) << 4) | (imm & 0x0F), 2, 1, [=]() { LdiImpl(ctx, Rd, imm); },
          std::format("LDI R{}, {}", Rd, imm)),
      ctx(ctx), Rd(Rd), imm(imm)
{
}

LDIMatcher::LDIMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t LDIMatcher::MinLength() const
{
    return 2;
}

bool LDIMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xF000) == 0xE000;
}

std::unique_ptr<Instruction> LDIMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t K = ((word >> 4) & 0xF0) | (word & 0x0F);
    uint8_t Rd = 16 + ((word >> 4) & 0x0F);
    return std::make_unique<LDI>(ctx, Rd, K);
}