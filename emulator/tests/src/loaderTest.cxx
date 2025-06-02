#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "memory.hxx"
#include "loader.hxx"

SCENARIO("HexProgramLoader correctly loads Intel HEX data into FlashMemory via MemoryController") {
    GIVEN("a MemoryController with registered FlashMemory") {
        auto flashMemory = std::make_unique<FlashMemory>(32768); // 32KB Flash for Arduino Uno
        auto* flashPtr = flashMemory.get();

        MemoryController memController;
        memController.RegisterMemory(std::move(flashMemory), 0x0000);

        HexProgramLoader loader;

        WHEN("a complete valid Intel HEX program is loaded") {
            const std::string hexContent =
                ":100000000C9434000C9451000C9451000C94510049\n"
                ":100010000C9451000C9451000C9451000C9451001C\n"
                ":100020000C9451000C9451000C9451000C9451000C\n"
                ":100030000C9451000C9451000C9451000C945100FC\n"
                ":100040000C9458000C9451000C9451000C945100E5\n"
                ":100050000C9451000C9451000C9451000C945100DC\n"
                ":100060000C9451000C94510011241FBECFEFD8E026\n"
                ":0A01CE003161626364656667310009\n"
                ":00000001FF\n";

            std::vector<unsigned char> programData(hexContent.begin(), hexContent.end());

            THEN("it loads without throwing exceptions") {
                REQUIRE_NOTHROW(loader.LoadProgram(programData, &memController));
            }

            THEN("FlashMemory contains expected data at various offsets") {
                loader.LoadProgram(programData, &memController);

                auto checkBytes = [&](uint64_t addr, std::vector<unsigned char> expected) {
                    auto actual = flashPtr->ReadLocation(addr, expected.size());
                    REQUIRE(actual == expected);
                };

                checkBytes(0x0000, {
                    0x0C, 0x94, 0x34, 0x00,
                    0x0C, 0x94, 0x51, 0x00,
                    0x0C, 0x94, 0x51, 0x00,
                    0x0C, 0x94, 0x51, 0x00
                });

                checkBytes(0x0060, {
                    0x0C, 0x94, 0x51, 0x00,
                    0x0C, 0x94, 0x51, 0x00,
                    0x11, 0x24, 0x1F, 0xBE,
                    0xCF, 0xEF, 0xD8, 0xE0
                });

                checkBytes(0x01CE, {
                    0x31, 0x61, 0x62, 0x63,
                    0x64, 0x65, 0x66, 0x67,
                    0x31, 0x00
                });
            }
        }

        WHEN("a HEX program with a bad checksum is loaded") {
            const std::string badHex = ":100000000C945C000C946E000C946E000C946E00FF\n"; // Invalid checksum
            std::vector<unsigned char> badData(badHex.begin(), badHex.end());

            THEN("an exception for checksum mismatch is thrown") {
                REQUIRE_THROWS_WITH(loader.LoadProgram(badData, &memController),
                                    Catch::Matchers::ContainsSubstring("Checksum mismatch"));
            }
        }

        WHEN("a HEX program with an unsupported record type is loaded") {
            const std::string unsupportedHex = ":020000040800F2\n"; // Record type 0x04
            std::vector<unsigned char> unsupportedData(unsupportedHex.begin(), unsupportedHex.end());

            THEN("an exception for unsupported record type is thrown") {
                REQUIRE_THROWS_WITH(loader.LoadProgram(unsupportedData, &memController),
                                    Catch::Matchers::ContainsSubstring("Unsupported HEX record type"));
            }
        }
    }
} 
