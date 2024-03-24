#include "../macro.hpp"

#include <filesystem>

namespace zephyrus::formats::GDR {

    /// @brief Convert a GDReplay macro file to a Zephyrus macro
    /// @note https://github.com/maxnut/GDReplayFormat
    bool readFromFile(const std::filesystem::path &path, Macro &macro);

    /// @brief Convert a Zephyrus macro to a GDReplay macro file
    void writeToFile(const Macro &macro, const std::filesystem::path &path);

}