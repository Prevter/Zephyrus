#include <zephyrus/macro.hpp>

namespace zephyrus {

    void Macro::clearFrames(uint32_t from) {
        m_frames.erase(std::remove_if(m_frames.begin(), m_frames.end(), [from](const Frame& frame) {
            return frame.getFrame() >= from;
        }), m_frames.end());
        m_frameFixes.erase(std::remove_if(m_frameFixes.begin(), m_frameFixes.end(), [from](const FrameFix& frameFix) {
            return frameFix.getFrame() >= from;
        }), m_frameFixes.end());
    }

    void Macro::addFrame(uint32_t frame, bool secondPlayer, PlayerButton button, bool pressed) {
        m_frames.emplace_back(frame, secondPlayer, button, pressed);
    }

    void Macro::addFrameFix(uint32_t frame, FrameFix::PlayerData player1) {
        m_frameFixes.emplace_back(frame, player1);
    }

    void Macro::addFrameFix(uint32_t frame, FrameFix::PlayerData player1, FrameFix::PlayerData player2) {
        m_frameFixes.emplace_back(frame, player1, player2);
    }

    std::vector<Macro::Frame> Macro::getFrames(uint32_t startFrame, uint32_t endFrame) const {
        std::vector<Macro::Frame> frames;
        for (const auto& frame : m_frames) {
            if (frame.getFrame() >= startFrame && frame.getFrame() <= endFrame) {
                frames.push_back(frame);
            }
        }
        return frames;
    }

    std::vector<Macro::Frame> Macro::getFrames(uint32_t frame) const {
        std::vector<Macro::Frame> frames;
        for (const auto& f : m_frames) {
            if (f.getFrame() == frame) {
                frames.push_back(f);
            }
        }
        return frames;
    }

    std::vector<Macro::FrameFix> Macro::getFrameFixes(uint32_t startFrame, uint32_t endFrame) const {
        std::vector<Macro::FrameFix> frameFixes;
        for (const auto& frameFix : m_frameFixes) {
            if (frameFix.getFrame() >= startFrame && frameFix.getFrame() <= endFrame) {
                frameFixes.push_back(frameFix);
            }
        }
        return frameFixes;
    }

    std::vector<Macro::FrameFix> Macro::getFrameFixes(uint32_t frame) const {
        std::vector<Macro::FrameFix> frameFixes;
        for (const auto& f : m_frameFixes) {
            if (f.getFrame() == frame) {
                frameFixes.push_back(f);
            }
        }
        return frameFixes;
    }

}