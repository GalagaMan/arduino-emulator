#include "instructions/encode.hxx"


uint16_t EncodeADD(uint8_t Rd, uint8_t Rr)
{
    return 0x0C00 | ((Rd & 0x1F) << 4) | (Rr & 0x0F) | ((Rr & 0x10) << 5);
}

uint16_t EncodeAND(uint8_t Rd, uint8_t Rr)
{
    return 0x2000 | ((Rd & 0x1F) << 4) | (Rr & 0x0F) | ((Rr & 0x10) << 5);
}

uint16_t EncodeANDI(uint8_t Rd, uint8_t K)
{
    return 0x7000 | ((Rd - 16) << 4) | (K & 0x0F) | ((K & 0xF0) << 4);
}

uint16_t EncodeBREQ(int8_t offset)
{
    return 0xF001 | ((offset & 0x3F) << 3);
}

uint16_t EncodeBRNE(int8_t offset)
{
    return 0xF401 | ((offset & 0x3F) << 3);
}

uint16_t EncodeCLR(uint8_t Rd)
{
    return EncodeEOR(Rd, Rd);
}

uint16_t EncodeCP(uint8_t Rd, uint8_t Rr)
{
    return 0x1400 | ((Rd & 0x1F) << 4) | (Rr & 0x0F) | ((Rr & 0x10) << 5);
}

uint16_t EncodeCPI(uint8_t Rd, uint8_t K)
{
    return 0x3000 | ((Rd - 16) << 4) | (K & 0x0F) | ((K & 0xF0) << 4);
}

uint16_t EncodeEOR(uint8_t Rd, uint8_t Rr)
{
    return 0x2400 | ((Rd & 0x1F) << 4) | (Rr & 0x0F) | ((Rr & 0x10) << 5);
}

uint16_t EncodeLDI(uint8_t Rd, uint8_t K)
{
    return 0xE000 | ((Rd - 16) << 4) | (K & 0x0F) | ((K & 0xF0) << 4);
}

uint16_t EncodeMOV(uint8_t Rd, uint8_t Rr)
{
    return 0x2C00 | ((Rd & 0x1F) << 4) | (Rr & 0x0F) | ((Rr & 0x10) << 5);
}

uint16_t EncodeNOP()
{
    return 0x0000;
}

uint16_t EncodeOR(uint8_t Rd, uint8_t Rr)
{
    return 0x2800 | ((Rd & 0x1F) << 4) | (Rr & 0x0F) | ((Rr & 0x10) << 5);
}

uint16_t EncodeORI(uint8_t Rd, uint8_t K)
{
    return 0x6000 | ((Rd - 16) << 4) | (K & 0x0F) | ((K & 0xF0) << 4);
}

uint16_t EncodePOP(uint8_t Rr)
{
    return 0x900F | ((Rr & 0x1F) << 4);
}

uint16_t EncodePUSH(uint8_t Rr)
{
    return 0x920F | ((Rr & 0x1F) << 4);
}

uint16_t EncodeRJMP(int16_t offset)
{
    return 0xC000 | (offset & 0x0FFF);
}

uint16_t EncodeSUB(uint8_t Rd, uint8_t Rr)
{
    return 0x1800 | ((Rd & 0x1F) << 4) | (Rr & 0x0F) | ((Rr & 0x10) << 5);
}

uint16_t EncodeSUBI(uint8_t Rd, uint8_t K)
{
    return 0x5000 | ((Rd - 16) << 4) | (K & 0x0F) | ((K & 0xF0) << 4);
}

uint8_t ExtractLow(uint16_t opcode)
{
    uint8_t low = opcode & 0xFFU;
    return low;
}

uint8_t ExtractHigh(uint16_t opcode)
{
    uint8_t high = opcode >> 8U;
    return high;
}
