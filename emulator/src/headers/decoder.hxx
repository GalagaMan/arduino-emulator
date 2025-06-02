#pragma once





class CPU;
class MemoryController;

class DecodeContext
{
private:
    CPU* cpu;
    MemoryController* memoryController;

public:
};


class Decoder
{
public:
    virtual ~Decoder() = default;
};