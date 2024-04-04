/**
 * @file mpt.hpp
 * @brief This file defines fake MPT.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MPT_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MPT_HPP_

#include <vector>

namespace data_types {
    /// @brief This is a fake MPT node replaced with a simple vector for now.
    template<typename T>
    using MPTNode = std::vector<T>;
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MPT_HPP_
