#include <zephyrus/file-io.hpp>

#include <fstream>
#include <bit>
#include <iostream>

#include <zephyrus/formats/gdreplay.hpp>

namespace zephyrus {

    /// @brief Utility namespace for reading files using little endian
    namespace FileReader {
        bool isBigEndian()
        {
            union {
                uint32_t i;
                char c[4];
            } bInt = {0x01020304};
            return bInt.c[0] == 1;
        }

        /// @brief Reads a value from a file using little endian
        template<typename T>
        T read(std::ifstream &file) {
            T value;
            file.read(reinterpret_cast<char *>(&value), sizeof(T));

            // If the system is big endian, swap the bytes
            if (isBigEndian()) {
                for (size_t i = 0; i < sizeof(T) / 2; i++) {
                    std::swap(reinterpret_cast<char *>(&value)[i], reinterpret_cast<char *>(&value)[sizeof(T) - i - 1]);
                }
            }

            return value;
        }

        double readDouble(std::ifstream &file) {
            double value;
            file.read(reinterpret_cast<char *>(&value), sizeof(double));
            return value;
        }

        float readSingle(std::ifstream &file) {
            float value;
            file.read(reinterpret_cast<char *>(&value), sizeof(float));
            return value;
        }

        Macro::FrameFix::PlayerData readPlayerData(std::ifstream &file) {
            Macro::FrameFix::PlayerData data{};
            data.x = FileReader::readSingle(file);
            data.y = FileReader::readSingle(file);
            data.ySpeed = FileReader::readDouble(file);
            data.rotation = FileReader::readSingle(file);
            return data;
        }

        MacroFileHeader readFileHeader(std::ifstream &file) {
            MacroFileHeader header{};
            header.magic = FileReader::read<uint16_t>(file);
            header.version = FileReader::read<uint8_t>(file);
            header.recordedFPS = FileReader::read<uint32_t>(file);
            header.actionCount = FileReader::read<uint32_t>(file);
            header.frameFixCount = FileReader::read<uint32_t>(file);
            return header;
        }

        MacroFileAction readFileAction(std::ifstream &file) {
            MacroFileAction action{};
            action.frame = FileReader::read<uint32_t>(file);
            action.flags = FileReader::read<uint8_t>(file);
            return action;
        }

        /// @brief Writes a value to a file using little endian
        template<typename T>
        void write(std::ofstream &file, T value) {
            // If the system is big endian, swap the bytes
            if (isBigEndian()) {
                for (size_t i = 0; i < sizeof(T) / 2; i++) {
                    std::swap(reinterpret_cast<char *>(&value)[i], reinterpret_cast<char *>(&value)[sizeof(T) - i - 1]);
                }
            }

            file.write(reinterpret_cast<char *>(&value), sizeof(T));
        }

        void write(std::ofstream &file, double value) {
            file.write(reinterpret_cast<char *>(&value), sizeof(double));
        }

        void write(std::ofstream &file, float value) {
            file.write(reinterpret_cast<char *>(&value), sizeof(float));
        }

        void writePlayerData(std::ofstream &file, const Macro::FrameFix::PlayerData &data) {
            FileReader::write(file, data.x);
            FileReader::write(file, data.y);
            FileReader::write(file, data.ySpeed);
            FileReader::write(file, data.rotation);
        }

        void writeFileHeader(std::ofstream &file, const MacroFileHeader &header) {
            FileReader::write(file, header.magic);
            FileReader::write(file, header.version);
            FileReader::write(file, header.recordedFPS);
            FileReader::write(file, header.actionCount);
            FileReader::write(file, header.frameFixCount);
        }

        void writeFileAction(std::ofstream &file, const MacroFileAction &action) {
            FileReader::write(file, action.frame);
            FileReader::write(file, action.flags);
        }
    }

    bool readFromFile(const std::filesystem::path &path, Macro &macro) {
        // Check if the file exists
        if (!std::filesystem::exists(path)) {
            return false;
        }

        // Deduce file format from file extension
        std::string extension = path.extension().string();
        if (extension == ".gdr") // Read GDReplay file
            return formats::GDR::readFromFile(path, macro);

        // If not a third-party format, read the file as a Zephyrus macro
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        MacroFileHeader header = FileReader::readFileHeader(file);

        if (header.magic != 0x525A || header.version != 2) {
            return false;
        }

        macro.clearFrames();

        for (uint32_t i = 0; i < header.actionCount; i++) {
            MacroFileAction action = FileReader::readFileAction(file);
            macro.addFrame(action.frame, action.isPlayer2(), action.getButton(), action.isButtonDown());
        }

        for (uint32_t i = 0; i < header.frameFixCount; i++) {
            MacroFileFrameFix frameFix{};
            frameFix.frame = FileReader::read<uint32_t>(file);
            frameFix.player1 = FileReader::readPlayerData(file);
            frameFix.player2Exists = FileReader::read<bool>(file);
            if (frameFix.player2Exists) {
                frameFix.player2 = FileReader::readPlayerData(file);
            }

            if (frameFix.player2Exists) {
                macro.addFrameFix(frameFix.frame, frameFix.player1, frameFix.player2);
            } else {
                macro.addFrameFix(frameFix.frame, frameFix.player1);
            }
        }

        return true;
    }

    void writeToFile(const Macro &macro, const std::filesystem::path &path) {
        // Deduce file format from file extension
        std::string extension = path.extension().string();
        if (extension == ".gdr") // Read GDReplay file
            return formats::GDR::writeToFile(macro, path);

        // If not a third-party format, write the file as a Zephyrus macro
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return;
        }

        MacroFileHeader header{};
        header.magic = 0x525A;
        header.version = 2;
        header.recordedFPS = 240;
        header.actionCount = macro.getFrames().size();
        header.frameFixCount = macro.getFrameFixes().size();

        FileReader::writeFileHeader(file, header);

        for (const auto &frame : macro.getFrames()) {
            MacroFileAction action{};
            action.frame = frame.getFrame();
            action.flags = (frame.isSecondPlayer() ? 0b10000000 : 0) |
                           (frame.isPressed() ? 0b01000000 : 0) |
                           (static_cast<uint8_t>(frame.getButton()) << 4);
            FileReader::writeFileAction(file, action);
        }

        for (const auto &frameFix : macro.getFrameFixes()) {
            MacroFileFrameFix fix{};
            fix.frame = frameFix.getFrame();
            fix.player1 = frameFix.getPlayer1();
            fix.player2Exists = frameFix.player2Exists();
            if (fix.player2Exists) {
                fix.player2 = frameFix.getPlayer2();
            }

            FileReader::write(file, fix.frame);
            FileReader::writePlayerData(file, fix.player1);
            FileReader::write(file, fix.player2Exists);
            if (fix.player2Exists) {
                FileReader::writePlayerData(file, fix.player2);
            }
        }
    }

}