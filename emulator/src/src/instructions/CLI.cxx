#include "instructions/CLI.hxx"
#include "cpu.hxx"

uint16_t EncodeCLI()
{
    return 0x94F8;
}

namespace
{
void CliImpl(AVRInstructionContext* ctx)
{
    uint8_t sreg = ctx->accessBehavior->GetSREG();
    sreg &= ~(1 << 7); // Clear I-bit
    ctx->accessBehavior->SetSREG(sreg);
    ctx->cpu->SetPC(ctx->cpu->GetPC() + 2);
}
} // namespace

CLI::CLI(AVRInstructionContext* ctx) : Instruction(0x94F8, 2, 1, [=]() { CliImpl(ctx); }, "CLI"), ctx(ctx)
{
}

CLIMatcher::CLIMatcher(AVRInstructionContext* ctx) : ctx(ctx)
{
}

size_t CLIMatcher::MinLength() const
{
    return 2;
}

bool CLIMatcher::Matches(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 2)
        return false;

    uint16_t word = bytes[0] | (bytes[1] << 8);
    return word == 0x94F8;
}

std::unique_ptr<Instruction> CLIMatcher::Decode(const std::vector<uint8_t>& bytes) const
{
    return std::make_unique<CLI>(ctx);
}
