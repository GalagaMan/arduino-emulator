#include "instructions/SUBI.hxx"
#include "cpu.hxx"
#include <format>


namespace
{
void SubiImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t imm)
{
    auto value = ctx->accessBehavior->GetRegister(Rd);
    auto result = value - imm;
    ctx->accessBehavior->SetRegister(Rd, result);

    uint8_t sreg = ctx->accessBehavior->GetSREG();
    // update Z flag
    if (result == 0)
        sreg |= (1 << 1); // Z
    else
        sreg &= ~(1 << 1);
    ctx->accessBehavior->SetSREG(sreg);

    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

SUBI::SUBI(AVRInstructionContext* ctx, uint8_t Rd, uint8_t imm)
    : Instruction(
          0x5000 | ((imm & 0xF0) << 4) | ((Rd - 16) << 4) | (imm & 0x0F), 2, 1, [=]() { SubiImpl(ctx, Rd, imm); },
          std::format("SUBI R{}, {}", Rd, imm)),
      ctx(ctx), Rd(Rd), imm(imm)
{
}

SUBIMatcher::SUBIMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t SUBIMatcher::MinLength() const
{
    return 2;
}

bool SUBIMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xF000) == 0x5000;
}

std::unique_ptr<Instruction> SUBIMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t K = ((word >> 4) & 0xF0) | (word & 0x0F);
    uint8_t Rd = 16 + ((word >> 4) & 0x0F);
    return std::make_unique<SUBI>(ctx, Rd, K);
}