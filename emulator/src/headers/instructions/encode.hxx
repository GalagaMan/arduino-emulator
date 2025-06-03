#pragma once
#include <cstdint>


uint16_t EncodeADD(uint8_t Rd, uint8_t Rr);
uint16_t EncodeAND(uint8_t Rd, uint8_t Rr);
uint16_t EncodeANDI(uint8_t Rd, uint8_t K);
uint16_t EncodeBREQ(int8_t offset);
uint16_t EncodeBRNE(int8_t offset);
uint16_t EncodeCLR(uint8_t Rd);
uint16_t EncodeCP(uint8_t Rd, uint8_t Rr);
uint16_t EncodeCPI(uint8_t Rd, uint8_t K);
uint16_t EncodeEOR(uint8_t Rd, uint8_t Rr);
uint16_t EncodeLDI(uint8_t Rd, uint8_t K);
uint16_t EncodeMOV(uint8_t Rd, uint8_t Rr);
uint16_t EncodeNOP();
uint16_t EncodeOR(uint8_t Rd, uint8_t Rr);
uint16_t EncodeORI(uint8_t Rd, uint8_t K);
uint16_t EncodePOP(uint8_t Rr);
uint16_t EncodePUSH(uint8_t Rr);
uint16_t EncodeRJMP(int16_t offset);
uint16_t EncodeSUB(uint8_t Rd, uint8_t Rr);
uint16_t EncodeSUBI(uint8_t Rd, uint8_t K);

uint8_t ExtractLow(uint16_t opcode);
uint8_t ExtractHigh(uint16_t opcode);