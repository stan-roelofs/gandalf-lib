#ifndef __GANDALF_SERIALIZATION_H
#define __GANDALF_SERIALIZATION_H

#include <cstdint>
#include <iostream>

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

        template <typename T>
        inline void Deserialize(std::istream& is, T& value)
        {
            static_assert(std::is_integral<T>::value, "T must be an integral type");
            value = 0;
            for (size_t byte = 0; byte < sizeof(T); ++byte) {
                value |= static_cast<T>(is.get()) << (byte * 8);
            }
        }
    }
}

#endif