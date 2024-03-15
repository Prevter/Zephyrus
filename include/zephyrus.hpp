#pragma once

#include <string>
#include <utility>
#include <vector>
#include <cstdint>
#include <functional>

#include "zephyrus/macro.hpp"
#include "zephyrus/file-io.hpp"

/// @brief The main namespace for the Zephyrus Replay Bot
namespace zephyrus {
    enum class BotState {
        Idle,
        Playing,
        Recording
    };

    enum class BotFixMode {
        None,
        EveryAction,
        EveryFrame
    };

    using HandleButtonMethod = std::function<void(int playerIndex, int buttonIndex, bool state)>;
    using FixPlayerMethod = std::function<void(int playerIndex, Macro::FrameFix::PlayerData data)>;
    using RequestMacroFixMethod = std::function<Macro::FrameFix()>;
    using GetFrameMethod = std::function<uint32_t()>;

    /// @brief The main class for the Zephyrus Replay Bot
    class Zephyrus {
    public: // Control methods
        /// @brief Sets the state of the bot
        void setState(BotState state);

        /// @brief Returns the state of the bot
        [[nodiscard]] BotState getState() const { return m_state; }

        /// @brief Sets the fix mode of the bot
        void setFixMode(BotFixMode fixMode) { m_fixMode = fixMode; }

        /// @brief Returns the fix mode of the bot
        [[nodiscard]] BotFixMode getFixMode() const { return m_fixMode; }

        /// @brief Set a macro for the bot to play
        void setMacro(const Macro& macro) { m_macro = macro; }

        /// @brief Returns the macro that the bot is playing
        [[nodiscard]] Macro& getMacro() { return m_macro; }

        /// @brief Sets the method to handle button presses
        void setHandleButtonMethod(HandleButtonMethod method) { m_handleButtonMethod = std::move(method); }

        /// @brief Returns the method to handle button presses
        [[nodiscard]] HandleButtonMethod getHandleButtonMethod() const { return m_handleButtonMethod; }

        /// @brief Sets the method to fix player data
        void setFixPlayerMethod(FixPlayerMethod method) { m_fixPlayerMethod = std::move(method); }

        /// @brief Returns the method to fix player data
        [[nodiscard]] FixPlayerMethod getFixPlayerMethod() const { return m_fixPlayerMethod; }

        /// @brief Sets the method to request a macro fix
        void setRequestMacroFixMethod(RequestMacroFixMethod method) { m_requestMacroFixMethod = std::move(method); }

        /// @brief Returns the method to request a macro fix
        [[nodiscard]] RequestMacroFixMethod getRequestMacroFixMethod() const { return m_requestMacroFixMethod; }

        /// @brief Returns the current frame
        [[nodiscard]] uint32_t getFrame() const { return m_frame; }

        /// @brief Sets the method to get the current frame
        [[nodiscard]] GetFrameMethod getGetFrameMethod() const { return m_getFrameMethod; }

        /// @brief Returns the method to get the current frame
        void setGetFrameMethod(GetFrameMethod method) { m_getFrameMethod = std::move(method); }

    protected:
        BotState m_state = BotState::Idle;
        BotFixMode m_fixMode = BotFixMode::EveryAction;
        uint32_t m_frame{};
        Macro m_macro;
        HandleButtonMethod m_handleButtonMethod;
        FixPlayerMethod m_fixPlayerMethod;
        RequestMacroFixMethod m_requestMacroFixMethod;
        GetFrameMethod m_getFrameMethod;

    public: // Hook callbacks
        /// @brief PlayerObject::pushButton hook
        /// @param playerIndex The index of the player (0 for player 1, 1 for player 2)
        /// @param buttonIndex The index of the button (1 for Jump, 2 for Left, 3 for Right)
        void PlayerObjectPushButton(int playerIndex, int buttonIndex);

        /// @brief PlayerObject::releaseButton hook
        /// @param playerIndex The index of the player (0 for player 1, 1 for player 2)
        /// @param buttonIndex The index of the button (1 for Jump, 2 for Left, 3 for Right)
        void PlayerObjectReleaseButton(int playerIndex, int buttonIndex);

        /// @brief GJBaseGameLayer::processCommands hook
        /// @param frame The current frame
        void GJBaseGameLayerProcessCommands();

        /// @brief PlayLayer::resetLevel hook
        /// @param frame Frame on which player got respawned
        void PlayLayerResetLevel();
    };
}