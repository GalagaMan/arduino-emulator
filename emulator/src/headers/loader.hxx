#pragma once
#include <vector>
// #include "board.hxx"

class MemoryController;

class ProgramLoader
{
public:    
    virtual ~ProgramLoader() = default;
    virtual void LoadProgram(std::vector<unsigned char> const& programData, MemoryController* memController) = 0;
};


class HexProgramLoader : public ProgramLoader
{
public:
    void LoadProgram(const std::vector<unsigned char> &programData, MemoryController *memController) override;
};