#include "instructions/POP.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <format>

namespace
{
void PopImpl(AVRInstructionContext* ctx, uint8_t Rr)
{
    // Get current SP
    uint16_t sp = (static_cast<uint16_t>(ctx->accessBehavior->GetSPH()) << 8) | ctx->accessBehavior->GetSPL();

    // Read from stack
    // auto val = ctx->memoryController->Read(typeid(SRAMMemory), sp, 1).at(0);
    auto val = ctx->accessBehavior->ReadFromSRAM(sp, 1).at(0);

    // Store into register
    ctx->accessBehavior->SetRegister(Rr, val);

    // Increment stack pointer
    ++sp;
    ctx->accessBehavior->SetSPL(static_cast<uint8_t>(sp & 0xFF));
    ctx->accessBehavior->SetSPH(static_cast<uint8_t>((sp >> 8) & 0xFF));

    // Advance PC
    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

POP::POP(AVRInstructionContext* ctx, uint8_t Rr)
    : Instruction(0x900F, 2, 2, [=]() { PopImpl(ctx, Rr); }, std::format("POP R{}", Rr)), ctx(ctx), Rr(Rr)
{
}

POPMatcher::POPMatcher(AVRInstructionContext* ctx) : ctx(ctx)
{
}

size_t POPMatcher::MinLength() const
{
    return 2;
}

bool POPMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return (word & 0xFE0F) == 0x900F;
}

std::unique_ptr<Instruction> POPMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word = bytes[0] | (bytes[1] << 8);
    uint8_t Rr = (word >> 4) & 0x1F;
    return std::make_unique<POP>(ctx, Rr);
}
