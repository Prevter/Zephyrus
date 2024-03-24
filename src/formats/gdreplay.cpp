#include <zephyrus/formats/gdreplay.hpp>

#include <fstream>
#include "../../thirdparty/json.hpp"

#include <iostream>

namespace zephyrus::formats::GDR {

    enum class GDRFormat {
        Unknown,
        MegaOverlay,
        MegaHack
    };

    bool readFromFile(const std::filesystem::path &path, Macro &macro) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        nlohmann::json json;

        // GDR can be in either JSON or MessagePack format
        // We'll try to parse it as JSON first
        json = nlohmann::json::parse(data, nullptr, false);
        if (json.is_discarded()) {
            // If it fails, try to parse it as MessagePack
            json = nlohmann::json::from_msgpack(data, true, false);
            if (json.is_discarded()) {
                return false;
            }
        }

        // Start parsing the JSON
        try {
            GDRFormat format = GDRFormat::Unknown;

            // Check whether it's MegaOverlay or MegaHack format (or neither)
            // This is important because there are differences in the format
            auto botName = json["bot"]["name"].get<std::string>();
            if (botName == "Macrobot") {
                format = GDRFormat::MegaOverlay;
            } else if (botName == "MH_REPLAY" || botName == "Zephyrus") {
                format = GDRFormat::MegaHack; // This bot uses the same format as MegaHack for compatibility
            }

            // Parse the actions
            auto inputs = json["inputs"];
            for (auto &input : inputs) {
                auto is2player = input["2p"].get<bool>();
                auto button = input["btn"].get<uint8_t>();
                auto isDown = input["down"].get<bool>();
                auto frame = input["frame"].get<uint32_t>();

                macro.addFrame(frame, is2player, static_cast<PlayerButton>(button), isDown);

                // Parse the frame fix
                if (format == GDRFormat::MegaHack) {
                    if (!input.contains("mhr_meta")) continue;

                    auto posX = input["mhr_x"].get<float>();
                    auto posY = input["mhr_y"].get<float>();
                    auto yVel = input["mhr_yvel"].get<double>();

                    macro.addFrameFix(frame, {posX, posY, yVel, 0});
                }
                else if (format == GDRFormat::MegaOverlay) {
                    if (!input.contains("correction")) continue;
                    auto correction = input["correction"];
                    if (correction["player2"].get<bool>()) continue;

                    frame = correction["frame"].get<uint32_t>();
                    auto posX = correction["xPos"].get<float>();
                    auto posY = correction["yPos"].get<float>();
                    auto yVel = correction["yVel"].get<double>();
                    auto rotation = correction["rotation"].get<float>();

                    macro.addFrameFix(frame, {posX, posY, yVel, rotation});
                }
            }

        } catch (const nlohmann::json::exception &e) {
            return false;
        }

        return true;
    }

    void writeToFile(const Macro &macro, const std::filesystem::path &path) {
        // Set metadata
        nlohmann::json json;
        json["author"] = "";
        json["bot"] = {
            {"name", "Zephyrus"},
            {"version", "2"}
        };
        json["coins"] = 0;
        json["description"] = "";
        json["duration"] = macro.getFrames().back().getFrame();
        json["gameVersion"] = 2.204;
        json["ldm"] = false;
        json["level"] = {
            {"id", 0},
            {"name", ""}
        };
        json["seed"] = rand(); // absolute, pls fix (make seed optional)
        json["version"] = 1.0;

        // Set inputs
        json["inputs"] = nlohmann::json::array();
        for (const auto &frame : macro.getFrames()) {
            // Check if the frame already exists
            if (!json["inputs"].empty()) {
                auto &lastFrame = json["inputs"].back();
                if (lastFrame["frame"].get<uint32_t>() == frame.getFrame()) {
                    continue;
                }
            }

            nlohmann::json input;
            input["2p"] = frame.isSecondPlayer();
            input["btn"] = static_cast<uint8_t>(frame.getButton());
            input["down"] = frame.isPressed();
            input["frame"] = frame.getFrame();

            // Set frame fix (use MegaHack format)
            auto frameFix = macro.getFrameFixes(frame.getFrame());
            if (!frameFix.empty()) {
                auto &fix = frameFix[0];
                input["mhr_meta"] = true;
                input["mhr_x"] = fix.getPlayer1().x;
                input["mhr_y"] = fix.getPlayer1().y;
                input["mhr_yvel"] = fix.getPlayer1().ySpeed;
            }

            json["inputs"].push_back(input);
        }

        std::ofstream file(path, std::ios::binary);
        file << json.dump(4);
        file.close();
    }

}