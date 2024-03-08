#pragma once

#include <filesystem>

#include "macro.hpp"

namespace zephyrus {

    /// @brief The header of a macro file
    struct MacroFileHeader {
        uint16_t magic{}; // Magic number to identify the file
        uint8_t version{}; // The version of the macro file
        uint32_t recordedFPS{}; // The FPS at which the macro was recorded
        uint32_t actionCount{}; // The number of actions in the macro
        uint32_t frameFixCount{}; // The number of frame fixes in the macro
    };

    /// @brief Contains information about a player action in a frame
    struct MacroFileAction {
        uint32_t frame; // The frame at which the action occurs
        uint8_t flags; // 1 bit (isPlayer2) | 1 bit (isButtonDown) | 2 bits (button)

        [[nodiscard]] bool isPlayer2() const { return flags & 0b10000000; }
        [[nodiscard]] bool isButtonDown() const { return flags & 0b01000000; }
        [[nodiscard]] PlayerButton getButton() const { return static_cast<PlayerButton>((flags & 0b00110000) >> 4); }
    };

    /// @brief Contains information about the state of the game to fix desyncs
    struct MacroFileFrameFix {
        uint32_t frame; // The frame at which the fix occurs
        Macro::FrameFix::PlayerData player1; // The data for player 1
        bool player2Exists; // Whether player 2 exists
        Macro::FrameFix::PlayerData player2; // The data for player 2 (if it exists)
    };

    /// @brief Reads a macro from a file
    /// @param path The path to the file
    /// @param macro The macro to read into
    /// @return True if the macro was read successfully, false otherwise
    bool readFromFile(const std::filesystem::path &path, Macro &macro);

    /// @brief Writes a macro to a file
    /// @param macro The macro to write
    /// @param path The path to the file
    void writeToFile(const Macro &macro, const std::filesystem::path &path);

}