#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_UTILS_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_UTILS_HPP_

#include <iostream>
#include <optional>

#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    /**
     * @brief Deserialize object from SSZ stream, which already can be deserialized from `bytes`.
     *
     * @tparam T must have `deserialize(const bytes& src)` method
     */
    template<class T>
    std::optional<T> deserialize_from_stream(std::istream& src) {
        std::optional<bytes> opt_bytes = read_bytes(src);
        if (!opt_bytes.has_value()) {
            return std::nullopt;
        }
        return T::deserialize(opt_bytes.value());
    }
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_UTILS_HPP_
