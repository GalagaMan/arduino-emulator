#include "instructions/CALL.hxx"
#include "cpu.hxx"
#include "memory.hxx"
#include <format>




uint16_t EncodeCALLLow(uint16_t addr)
{
    return addr & 0xFFFF;
}

uint16_t EncodeCALLHigh(uint16_t addr)
{
    return 0x940E | ((addr >> 16) & 0x01F0);
}

namespace
{
void CallImpl(AVRInstructionContext* ctx, uint16_t addr)
{
    auto pc = ctx->cpu->GetPC() + 4;
    auto high = static_cast<uint8_t>((pc >> 8) & 0xFF);
    auto low = static_cast<uint8_t>(pc & 0xFF);

    uint16_t sp = (ctx->accessBehavior->GetSPH() << 8) | ctx->accessBehavior->GetSPL();
    ctx->accessBehavior->WriteToSRAM(sp - 1, {high});
    ctx->accessBehavior->WriteToSRAM(sp - 2, {low});
    sp -= 2;
    ctx->accessBehavior->SetSPH(sp >> 8);
    ctx->accessBehavior->SetSPL(sp & 0xFF);

    ctx->cpu->SetPC(addr);
}
} // namespace

CALL::CALL(AVRInstructionContext* ctx, uint16_t addr)
    : Instruction(
          0x940E, 4, 4, [=]() { CallImpl(ctx, addr); }, std::format("CALL 0x{:04X}", addr)),
      ctx(ctx), addr(addr)
{
}

CALLMatcher::CALLMatcher(AVRInstructionContext* ctx) : ctx(ctx)
{
}

size_t CALLMatcher::MinLength() const
{
    return 4;
}

bool CALLMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 4)
        return false;

    uint16_t word1 = bytes[0] | (bytes[1] << 8);
    return (word1 & 0xFE0E) == 0x940E;
}

std::unique_ptr<Instruction> CALLMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    uint16_t word1 = bytes[0] | (bytes[1] << 8);
    uint16_t word2 = bytes[2] | (bytes[3] << 8);
    uint32_t addr = ((word1 & 0x01F0) << 8) | word2;

    return std::make_unique<CALL>(ctx, static_cast<uint16_t>(addr));
}