#ifndef __GANDALF_SERIALIZATION_H
#define __GANDALF_SERIALIZATION_H

#include <array>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>

namespace gandalf
{
    namespace serialization
    {
        template <typename T>
        inline void Serialize(std::ostream& os, T value)
        {
            static_assert(std::is_integral<T>::value, "T must be an integral type");
            for (size_t byte = 0; byte < sizeof(T); ++byte) {
                os.put(static_cast<char>(value & 0xFF));
                value >>= 8;
            }
        }

        template <typename T, std::size_t N>
        inline void Serialize(std::ostream& os, const std::array<T, N>& values)
        {
            for (const auto& value : values)
                Serialize(os, value);
        }

        template <typename T>
        inline void Serialize(std::ostream& os, const std::vector<T>& values)
        {
            Serialize(os, values.size());
            for (const auto& value : values)
                Serialize(os, value);
        }

        template <typename T>
        inline void Deserialize(std::istream& is, T& value)
        {
            static_assert(std::is_integral<T>::value, "T must be an integral type");
            value = 0;
            for (size_t byte = 0; byte < sizeof(T); ++byte) {
                value |= static_cast<T>(is.get()) << (byte * 8);
            }
        }

        template <typename T, std::size_t N>
        inline void Deserialize(std::istream& is, std::array<T, N>& values)
        {
            for (auto& value : values)
                Deserialize(is, value);
        }

        template <typename T>
        inline void Deserialize(std::istream& is, std::vector<T>& values)
        {
            std::size_t size;
            Deserialize(is, size);
            values.resize(size);
            for (auto& value : values)
                Deserialize(is, value);
        }
    }
}

#endif