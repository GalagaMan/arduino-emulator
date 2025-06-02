#include "catch2/matchers/catch_matchers_string.hpp"
#include "memory.hxx"
#include <catch2/catch_all.hpp>


class ROMMemory : public Memory
{
    std::vector<unsigned char> storage;

public:
    explicit ROMMemory(size_t size) : storage(size, 0xFF)
    {
    }

    void WriteLocation(uint64_t, std::vector<unsigned char> const&) override
    {
        throw std::runtime_error("ROM is read-only");
    }

    std::vector<unsigned char> ReadLocation(uint64_t addr, size_t size) const override
    {
        if (addr + size > storage.size())
            throw std::out_of_range("ROM read OOB");
        return {storage.begin() + addr, storage.begin() + addr + size};
    }

    size_t Size() const override
    {
        return storage.size();
    }
};

TEST_CASE("MemoryController advanced usage", "[MemoryController]")
{
    MemoryController controller;

    SECTION("Supports multiple memory types and regions")
    {
        controller.RegisterMemory(std::make_unique<FlashMemory>(64), 0x0000);
        controller.RegisterMemory(std::make_unique<SRAMMemory>(64), 0x0100);
        controller.RegisterMemory(std::make_unique<ROMMemory>(32), 0x0200);

        REQUIRE_NOTHROW(controller.Write<FlashMemory>(0x0000, {0xAA, 0xBB}));
        REQUIRE(controller.Read<FlashMemory>(0x0000, 2) == std::vector<unsigned char>{0xAA, 0xBB});

        REQUIRE_NOTHROW(controller.Write<SRAMMemory>(0x0100, {0x11, 0x22}));
        REQUIRE(controller.Read<SRAMMemory>(0x0100, 2) == std::vector<unsigned char>{0x11, 0x22});

        REQUIRE(controller.Read<ROMMemory>(0x0200, 2) == std::vector<unsigned char>{0xFF, 0xFF});
        REQUIRE_THROWS_WITH(controller.Write<ROMMemory>(0x0200, {0x01}),
                            Catch::Matchers::ContainsSubstring("read-only"));
    }

    SECTION("Handles multiple blocks of same type")
    {
        controller.RegisterMemory(std::make_unique<SRAMMemory>(32), 0x0000);
        controller.RegisterMemory(std::make_unique<SRAMMemory>(32), 0x0020);

        REQUIRE_NOTHROW(controller.Write<SRAMMemory>(0x0010, {0xAB, 0xCD}));
        REQUIRE(controller.Read<SRAMMemory>(0x0010, 2) == std::vector<unsigned char>{0xAB, 0xCD});

        REQUIRE_NOTHROW(controller.Write<SRAMMemory>(0x0025, {0x12}));
        REQUIRE(controller.Read<SRAMMemory>(0x0025, 1) == std::vector<unsigned char>{0x12});
    }

    SECTION("Overlapping regions allowed across types, data resolves correctly")
    {
        // Overlapping address space: both start at 0x0000, cover 32 bytes

        auto sram = std::make_unique<SRAMMemory>(32);
        auto flash = std::make_unique<FlashMemory>(32);

        Memory* sramptr = sram.get();
        Memory* flashptr = flash.get();

        controller.RegisterMemory(std::move(sram), 0x0000);
        controller.RegisterMemory(std::move(flash), 0x0000);

        // Write 0xAA into Flash at 0x0000
        controller.Write<FlashMemory>(0x0000, {0xAA});
        // Write 0x55 into SRAM at 0x0000
        controller.Write<SRAMMemory>(0x0000, {0x55});

        // Read both values back — ensure each memory retains its own data
        REQUIRE(controller.Read<FlashMemory>(0x0000, 1)[0] == 0xAA);
        REQUIRE(controller.Read<SRAMMemory>(0x0000, 1)[0] == 0x55);

        // Write 0xBB to 0x0010 in SRAM
        controller.Write<SRAMMemory>(0x0010, {0xBB});
        // Write 0xCC to 0x0010 in Flash
        controller.Write<FlashMemory>(0x0010, {0xCC});

        // Cross-check
        REQUIRE(controller.Read<SRAMMemory>(0x0010, 1)[0] == 0xBB);
        REQUIRE(controller.Read<FlashMemory>(0x0010, 1)[0] == 0xCC);
    }

    SECTION("Accessing out-of-bound addresses throws")
    {
        controller.RegisterMemory(std::make_unique<SRAMMemory>(32), 0x2000);

        REQUIRE_THROWS(controller.Read<SRAMMemory>(0x2000 + 40, 8));
        REQUIRE_THROWS(controller.Write<SRAMMemory>(0x2000 + 31, {0x01, 0x02}));
    }

    SECTION("Fully fills and retrieves memory range")
    {
        controller.RegisterMemory(std::make_unique<SRAMMemory>(16), 0x3000);
        std::vector<unsigned char> full = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
        controller.Write<SRAMMemory>(0x3000, full);
        REQUIRE(controller.Read<SRAMMemory>(0x3000, 16) == full);
    }

    SECTION("Byte-by-byte access in valid ranges works correctly")
    {
        controller.RegisterMemory(std::make_unique<SRAMMemory>(8), 0x4000);

        for (uint64_t i = 0; i < 8; ++i)
            controller.Write<SRAMMemory>(0x4000 + i, {static_cast<unsigned char>(i)});

        for (uint64_t i = 0; i < 8; ++i)
            REQUIRE(controller.Read<SRAMMemory>(0x4000 + i, 1)[0] == static_cast<unsigned char>(i));
    }
}