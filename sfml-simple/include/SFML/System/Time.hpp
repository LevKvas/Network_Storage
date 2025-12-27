#pragma once

#include "../Config.hpp"

namespace sf {

    class Time {
    private:
        Int64 m_microseconds;

    public:
        Time() : m_microseconds(0) {}
        explicit Time(Int64 microseconds) : m_microseconds(microseconds) {}

        float asSeconds() const { return m_microseconds / 1000000.0f; }
        Int32 asMilliseconds() const { return static_cast<Int32>(m_microseconds / 1000); }
        Int64 asMicroseconds() const { return m_microseconds; }

        static Time seconds(float amount) {
            return Time(static_cast<Int64>(amount * 1000000));
        }

        static Time milliseconds(Int32 amount) {
            return Time(static_cast<Int64>(amount) * 1000);
        }

        static Time microseconds(Int64 amount) {
            return Time(amount);
        }

        static const Time Zero;
    };

    const Time Time::Zero = Time();

} // namespace sf