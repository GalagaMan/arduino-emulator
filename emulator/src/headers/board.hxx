#pragma once
#include <memory>
#include <vector>



class CPU;
class MemoryController;


class Board
{
private:
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<MemoryController> memoryController;

public:
    void FlushProgram();
};