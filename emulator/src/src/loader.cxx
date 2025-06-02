#include "loader.hxx"
#include <stdexcept>
#include "memory.hxx"


namespace
{
uint8_t ParseHexByte(char high, char low)
{
    auto hexToInt = [](char c) -> uint8_t {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        throw std::runtime_error("Invalid hex character");
    };

    return (hexToInt(high) << 4) | hexToInt(low);
}

bool VerifyChecksum(const std::vector<uint8_t>& record)
{
    uint8_t sum = 0;
    for (size_t i = 0; i < record.size() - 1; ++i)
        sum += record[i];
    return ((~sum + 1) & 0xFF) == record.back();
}
} // namespace

#include <iostream>

void HexProgramLoader::LoadProgram(const std::vector<unsigned char>& programData, MemoryController* memController)
{
    std::istringstream stream(std::string(programData.begin(), programData.end()));
    std::string line;

    size_t lineCount{};

    while (std::getline(stream, line))
    {
        ++lineCount;
        std::cout << "Parsing line #" << lineCount << ": " << line << "\n";

        if (line.empty() || line[0] != ':')
            throw std::runtime_error("Invalid HEX line");

        if (line.size() < 11) // minimal line :llaaaattcc
            throw std::runtime_error("Invalid HEX line length");

        std::vector<uint8_t> record;
        for (size_t i = 1; i < line.size(); i += 2)
            record.push_back(ParseHexByte(line[i], line[i + 1]));

        if (!VerifyChecksum(record))
            throw std::runtime_error("Checksum mismatch in HEX record");

        uint8_t len = record[0];
        uint16_t address = (record[1] << 8) | record[2];
        uint8_t type = record[3];

        if (type == 0x00) // Data record
        {
            if (record.size() < (5 + len))
                throw std::runtime_error("Data length mismatch in HEX record");

            std::vector<unsigned char> data(record.begin() + 4, record.begin() + 4 + len);

            memController->Write<FlashMemory>(address, data);
        }
        else if (type == 0x01) // EOF
        {
            break;
        }
        else
        {
            throw std::runtime_error("Unsupported HEX record type encountered");
        }
    }

/*     auto* unit = memController->GetMemoryUnit<FlashMemory>(0);
    auto read = memController->Read<FlashMemory>(0, unit->Size());

    std::println(std::cout, "Read {} loaded bytes from FlashMemory", read.size());

    for (auto byte : read)
    {
        std::cout << std::hex << static_cast<int>(byte);
    } */
}