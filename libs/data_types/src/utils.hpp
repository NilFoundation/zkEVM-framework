/**
 * @file utils.hpp
 * @brief This file helpful internal functions.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_UTILS_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_UTILS_HPP_

#include <exception>
#include <expected>
#include <iostream>
#include <optional>

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/errors.hpp"

namespace data_types {
    namespace utils {
        /**
         * @brief Call ssz::serialize catching exceptions and turning them into errors.
         *
         * @tparam T type to serialize
         *
         * @return serialized bytes or error
         */
        template<class T>
        std::expected<bytes, SerializationError> serialize(const T& obj) noexcept {
            try {
                return ssz::serialize<T>(obj);
            } catch (const std::exception& e) {
                return std::unexpected(
                    SerializationError(SerializationError::SSZErrorType, e.what()));
            }
        }

        /**
         * @brief Call utils::serialize and then write result into stream.
         *
         * @tparam T type to serialize
         *
         * @return error if some
         */
        template<class T>
        std::expected<void, SerializationError> serialize_to_stream(const T& obj,
                                                                    std::ostream& out) noexcept {
            std::expected<bytes, SerializationError> result = serialize<T>(obj);
            bytes blob = TRYX(result);
            return write_bytes(blob, out);
        }

        /**
         * @brief Call ssz::deserialize catching the exception and turning it into error.
         *
         * @tparam T type to deserialize
         *
         * @return serialized bytes or error
         */
        template<class T>
        std::expected<T, SerializationError> deserialize(const bytes& src) noexcept {
            try {
                return ssz::deserialize<T>(src);
            } catch (const std::exception& e) {
                return std::unexpected(
                    SerializationError(SerializationError::SSZErrorType, e.what()));
            }
        }

        /**
         * @brief Read bytes from stream and then call utils::deserialize on them.
         *
         * @tparam T type to deserialize
         *
         * @return deserialized object or error
         */
        template<class T>
        std::expected<T, SerializationError> deserialize_from_stream(std::istream& src) noexcept {
            bytes blob = TRYX(read_bytes(src));
            return deserialize<T>(blob);
        }
    }  // namespace utils
}  // namespace data_types

#define DEFAULT_IMPL_SER(OriginType, ContainerType)                                   \
    std::expected<bytes, SerializationError> OriginType::serialize() const noexcept { \
        return utils::serialize<ContainerType>(ContainerType(*this));                 \
    }

#define DEFAULT_IMPL_SER_STREAM(OriginType, ContainerType)                           \
    std::expected<void, SerializationError> OriginType::serialize(std::ostream& out) \
        const noexcept {                                                             \
        return utils::serialize_to_stream<ContainerType>(ContainerType(*this), out); \
    }

#define DEFAULT_IMPL_DESER(OriginType, ContainerType)                        \
    std::expected<OriginType, SerializationError> OriginType::deserialize(   \
        const bytes& src) noexcept {                                         \
        ContainerType result = TRYX(utils::deserialize<ContainerType>(src)); \
        return static_cast<OriginType>(result);                              \
    }

#define DEFAULT_IMPL_DESER_STREAM(OriginType, ContainerType)                             \
    std::expected<OriginType, SerializationError> OriginType::deserialize(               \
        std::istream& src) noexcept {                                                    \
        ContainerType result = TRYX(utils::deserialize_from_stream<ContainerType>(src)); \
        return static_cast<OriginType>(result);                                          \
    }

/**
 * @brief This macro helps implementing SSZ (de-)serialization methods for common cases.
 *
 * This is needed just to save time because almost all Data Types have similar implementations
 * of these methods.
 */
#define DEFAULT_IMPL_SERDE_METHODS(OriginType, ContainerType) \
    DEFAULT_IMPL_SER(OriginType, ContainerType)               \
    DEFAULT_IMPL_SER_STREAM(OriginType, ContainerType)        \
    DEFAULT_IMPL_DESER(OriginType, ContainerType)             \
    DEFAULT_IMPL_DESER_STREAM(OriginType, ContainerType)

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_UTILS_HPP_
