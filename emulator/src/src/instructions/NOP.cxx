#include "instructions/NOP.hxx"
#include "instruction.hxx"
#include "cpu.hxx"


namespace
{

void NopImpl(AVRInstructionContext* ctx, size_t increment)
{
    auto* cpu = ctx->cpu;

    cpu->SetPC(cpu->GetPC() + increment);
}

} // namespace

NOP::NOP(AVRInstructionContext* ctx)
    : Instruction(
          0x0000, 2, 1, [ctx, this]() { NopImpl(ctx, Size()); }, "NOP"),
      ctx(ctx)
{
}


NOPMatcher::NOPMatcher(AVRInstructionContext* ctx)
    : ctx(ctx)
{
}

size_t NOPMatcher::MinLength() const
{
    return 2;
}

bool NOPMatcher::Matches(std::vector<uint8_t> const& bytes) const
{
    if (bytes.size() < MinLength())
        return false;

    auto match = bytes[0] == 0 && bytes[1] == 0;

    return match;
}

[[nodiscard]] std::unique_ptr<Instruction> NOPMatcher::Decode([[maybe_unused]] std::vector<uint8_t> const& bytes) const
{
    return std::make_unique<NOP>(ctx);
}
