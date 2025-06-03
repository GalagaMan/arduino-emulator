#include "instructions/MOV.hxx"
#include "cpu.hxx"
#include <format>




namespace
{
void MovImpl(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr)
{
    auto value = ctx->accessBehavior->GetRegister(Rr);
    ctx->accessBehavior->SetRegister(Rd, value);
    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

MOV::MOV(AVRInstructionContext* ctx, uint8_t Rd, uint8_t Rr)
    : Instruction(
          0x2C00, 2, 1, [=]() { MovImpl(ctx, Rd, Rr); }, std::format("MOV R{}, R{}", Rd, Rr)),
      ctx(ctx), Rd(Rd), Rr(Rr)
{
}


MOVMatcher::MOVMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t MOVMatcher::MinLength() const
{
    return 2;
}

bool MOVMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xFC00) == 0x2C00;
}

std::unique_ptr<Instruction> MOVMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);

    uint8_t Rd = ((word >> 4) & 0x1F);
    uint8_t Rr = (word & 0x0F) | ((word >> 5) & 0x10);

    return std::make_unique<MOV>(ctx, Rd, Rr);
}
