#include <zephyrus.hpp>

#include <iostream>

namespace zephyrus {
    void Zephyrus::setState(BotState state) {
        m_state = state;
    }

    void Zephyrus::PlayerObjectPushButton(int playerIndex, int buttonIndex) {
        if (m_state == BotState::Recording) {
            m_macro.addFrame(m_frame, playerIndex, static_cast<PlayerButton>(buttonIndex), true);
        }
    }

    void Zephyrus::PlayerObjectReleaseButton(int playerIndex, int buttonIndex) {
        if (m_state == BotState::Recording) {
            m_macro.addFrame(m_frame, playerIndex, static_cast<PlayerButton>(buttonIndex), false);
        }
    }

    void Zephyrus::GJBaseGameLayerProcessCommands() {
        uint32_t frame = m_getFrameMethod();
        if (frame == m_frame) return;

        uint32_t oldFrame = m_frame;
        uint32_t frameDiff = frame - m_frame;
        m_frame = frame;

        if (m_state == BotState::Playing) {
            auto frames = frameDiff > 1 ? // If the frame difference is greater than 1
                    m_macro.getFrames(oldFrame + 1, m_frame) : // Get all frames between the last frame and the current frame
                    m_macro.getFrames(m_frame); // Get only the current frame
            for (const auto &f: frames) {
                m_handleButtonMethod(
                        f.isSecondPlayer() ? 1 : 0,
                        static_cast<int>(f.getButton()),
                        f.isPressed());
            }

            if (m_fixMode == BotFixMode::EveryFrame || (m_fixMode == BotFixMode::EveryAction && !frames.empty())) {
                auto frameFixes = m_macro.getFrameFixes(m_frame);
                for (const auto &f: frameFixes) {
                    m_fixPlayerMethod(0, f.getPlayer1());
                    if (f.player2Exists())
                        m_fixPlayerMethod(1, f.getPlayer2());
                }
            }
        } else if (m_state == BotState::Recording) {
             Macro::FrameFix playerData = m_requestMacroFixMethod();
             if (playerData.player2Exists()) {
                 m_macro.addFrameFix(m_frame, playerData.getPlayer1(), playerData.getPlayer2());
             } else {
                 m_macro.addFrameFix(m_frame, playerData.getPlayer1());
             }
        }
    }

    void Zephyrus::PlayLayerResetLevel() {
        uint32_t frame = m_getFrameMethod();

        if (m_state == BotState::Recording) {
            // Remove everything past the current frame
            m_macro.clearFrames(frame);
        }
    }
}