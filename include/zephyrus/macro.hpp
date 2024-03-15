#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace zephyrus {

    enum class PlayerButton {
        Jump = 1,
        Left = 2,
        Right = 3
    };

    /// @brief A class that defines a macro and contains information about all the frames
    class Macro {
    public:

        /// @brief A class that defines one frame of a macro and contains information about player input
        class Frame {
        public:
            Frame(uint32_t frame, bool secondPlayer, PlayerButton button, bool pressed) :
                    m_frame(frame), m_secondPlayer(secondPlayer), m_button(button), m_pressed(pressed) {}

            [[nodiscard]] uint32_t getFrame() const { return m_frame; }

            [[nodiscard]] bool isSecondPlayer() const { return m_secondPlayer; }

            [[nodiscard]] PlayerButton getButton() const { return m_button; }

            [[nodiscard]] bool isPressed() const { return m_pressed; }

        protected:
            uint32_t m_frame;
            bool m_secondPlayer;
            PlayerButton m_button;
            bool m_pressed;
        };

        /// @brief A class that defines a frame fix and contains information about the state of the game to fix desyncs
        class FrameFix {
        public:
            /// @brief A struct that contains information about a player's data in a frame
            struct PlayerData {
                float x; // The x position of the player
                float y; // The y position of the player
                double ySpeed; // The y speed of the player
                float rotation; // The rotation of the player
            };

            FrameFix(uint32_t frame, PlayerData player1) :
                    m_frame(frame), m_player1(player1), m_player2Exists(false) {}

            FrameFix(uint32_t frame, PlayerData player1, PlayerData player2) :
                    m_frame(frame), m_player1(player1), m_player2Exists(true), m_player2(player2) {}

            [[nodiscard]] uint32_t getFrame() const { return m_frame; }

            [[nodiscard]] const PlayerData &getPlayer1() const { return m_player1; }

            [[nodiscard]] bool player2Exists() const { return m_player2Exists; }

            [[nodiscard]] const PlayerData &getPlayer2() const { return m_player2; }

        protected:
            uint32_t m_frame;
            PlayerData m_player1;
            bool m_player2Exists;
            PlayerData m_player2{};
        };

        /// @brief Adds a frame to the macro
        void addFrame(uint32_t frame, bool secondPlayer, PlayerButton button, bool pressed);

        /// @brief Clears all the frames in the macro
        inline void clearFrames() {
            m_frames.clear();
            m_frameFixes.clear();
        }

        /// @brief Clears all the frames in the macro from the specified frame
        void clearFrames(uint32_t from);

        /// @brief Adds a frame fix to the macro with the data for player 1
        void addFrameFix(uint32_t frame, FrameFix::PlayerData player1);

        /// @brief Adds a frame fix to the macro with the data for both players
        void addFrameFix(uint32_t frame, FrameFix::PlayerData player1, FrameFix::PlayerData player2);

        /// @brief Returns all the frames in the macro
        [[nodiscard]] const std::vector<Frame> &getFrames() const { return m_frames; }

        /// @brief Returns all the frames in the macro between the start and end frames
        [[nodiscard]] std::vector<Frame> getFrames(uint32_t startFrame, uint32_t endFrame) const;

        /// @brief Returns all the frames in the macro at the specified frame
        [[nodiscard]] std::vector<Frame> getFrames(uint32_t frame) const;

        /// @brief Returns all the frame fixes in the macro
        [[nodiscard]] const std::vector<FrameFix> &getFrameFixes() const { return m_frameFixes; }

        /// @brief Returns all the frame fixes in the macro between the start and end frames
        [[nodiscard]] std::vector<FrameFix> getFrameFixes(uint32_t startFrame, uint32_t endFrame) const;

        /// @brief Returns all the frame fixes in the macro at the specified frame
        [[nodiscard]] std::vector<FrameFix> getFrameFixes(uint32_t frame) const;

    protected:
        std::vector<Frame> m_frames;
        std::vector<FrameFix> m_frameFixes;
    };


}