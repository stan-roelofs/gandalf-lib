#ifndef __GANDALF_SERIALIZATION_H
#define __GANDALF_SERIALIZATION_H

#include <array>
#include <cstdint>
#include <deque>
#include <iostream>
#include <map>
#include <type_traits>
#include <vector>

namespace gandalf
{
    class Serializable
    {
    public:
        /// Serializes the snapshot to the given stream
        virtual void Serialize(std::ostream& os) const = 0;

        /// Deserializes the snapshot from the given stream
        virtual void Deserialize(std::istream& is, std::uint16_t version) = 0;
    };

    class SerializationException: public std::exception
    {
    public:
        SerializationException(const std::string& message): message_(message) {}
        virtual const char* what() const { return message_.c_str(); }

    private:
        std::string message_;
    };

    namespace serialization
    {
        template <typename T>
        inline void Serialize(std::ostream& os, T value)
        {
            if constexpr (std::is_same<T, bool>::value) {
                os.put(value ? 1 : 0);
                if (os.fail())
                    throw SerializationException("Failed to serialize bool");
            }
            else if constexpr (std::is_base_of<Serializable, T>::value)
                value.Serialize(os);
            else
            {
                static_assert(std::is_integral<T>::value, "T must be an integral type");
                for (size_t i = 0; i < sizeof(T); ++i)
                {
                    os.put(static_cast<char>(value & 0xFF));
                    value >>= 8;
                    if (os.fail())
                        throw SerializationException("Failed to serialize");
                }
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
        inline void Serialize(std::ostream& os, const std::deque<T>& values)
        {
            Serialize(os, values.size());
            for (const auto& value : values)
                Serialize(os, value);
        }

        template <typename K, typename V>
        inline void Serialize(std::ostream& os, const std::map<K, V>& values)
        {
            Serialize(os, values.size());
            for (const auto& [key, value] : values)
            {
                Serialize(os, key);
                Serialize(os, value);
            }
        }

        template <typename T>
        inline void Deserialize(std::istream& is, T& value, std::uint16_t version = 0)
        {
            if constexpr (std::is_same<T, bool>::value)
            {
                value = is.get() != 0;
                if (is.fail())
                    throw SerializationException("Failed to deserialize bool");
            }
            else if constexpr (std::is_base_of<Serializable, T>::value)
            {
                value.Deserialize(is, version);
            }
            else {
                static_assert(std::is_integral<T>::value, "T must be an integral type");

                value = 0;
                for (size_t byte = 0; byte < sizeof(T); ++byte) {
                    value |= static_cast<T>(is.get()) << (byte * 8);
                    if (is.fail())
                        throw SerializationException("Failed to deserialize integral type");
                }
            }
        }

        template <typename T, std::size_t N>
        inline void Deserialize(std::istream& is, std::array<T, N>& values, std::uint16_t version = 0)
        {
            for (auto& value : values)
                Deserialize(is, value, version);
        }

        template <typename T>
        inline void Deserialize(std::istream& is, std::vector<T>& values, std::uint16_t version = 0)
        {
            std::size_t size;
            Deserialize(is, size);
            values.resize(size);
            for (auto& value : values)
                Deserialize(is, value, version);
        }

        template <typename T>
        inline void Deserialize(std::istream& is, std::deque<T>& values, std::uint16_t version = 0)
        {
            std::size_t size;
            Deserialize(is, size);
            values.resize(size);
            for (auto& value : values)
                Deserialize(is, value, version);
        }

        template <typename K, typename V>
        inline void Deserialize(std::istream& is, std::map<K, V>& values, std::uint16_t version = 0)
        {
            std::size_t size;
            Deserialize(is, size, version);
            for (std::size_t i = 0; i < size; ++i)
            {
                K key;
                V value;
                Deserialize(is, key);
                Deserialize(is, value);
                values.emplace(key, value);
            }
        }
    }
}

#endif