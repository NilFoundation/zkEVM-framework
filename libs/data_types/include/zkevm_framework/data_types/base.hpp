#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BASE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BASE_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <iostream>
#include <vector>

#include "zkevm_framework/data_types/errors.hpp"

namespace data_types {
    /// @brief Vector of bytes.
    using bytes = std::vector<std::byte>;

    /**
     * @brief Read bytes from input char stream.
     *
     * @return bytes read. IOError if any I/O error occured during reading from istream.
     */
    std::expected<bytes, IOError> read_bytes(std::istream& in) noexcept;

    /**
     * @brief Write bytes to output char stream.
     *
     * @return IOError if any I/O error occured during writing into ostream.
     */
    std::expected<void, IOError> write_bytes(const bytes& src, std::ostream& out) noexcept;

    /// @brief Size of account address in bytes.
    constexpr size_t address_size = 20;

    /// @brief Account address.
    using Address = std::array<uint8_t, address_size>;

    /// @brief Size of value in bytes.
    constexpr size_t value_size = 32;

    /// @brief Account address.
    using Value = std::array<uint8_t, value_size>;

    /// @brief Hash.
    using Hash = uint64_t;

    /// @brief Size of bloom in bytes.
    constexpr size_t bloom_size = 256;

    /// @brief Bloom.
    using Bloom = std::array<uint8_t, bloom_size>;

    /// @brief Hasher for `Address`.
    struct AddressHasher {
        size_t operator()(const Address& addr) const {
            size_t hash = 0;
            for (uint8_t b : addr) {
                hash ^= std::hash<uint8_t>{}(b);
            }
            return hash;
        }
    };

    /// @brief Size of ssz hash in bytes.
    constexpr size_t ssz_hash_size = 32;

    /// @brief SSZ merkalization hash.
    using SSZHash = std::array<std::byte, 32>;

    /// @brief Currency balance info
    using CurrencyBalance = std::array<std::byte, 24>;

    /// @brief Size of data in bytes.
    constexpr size_t data_size = 24;

    /// @brief Raw data
    using Data = std::array<uint8_t, data_size>;

    /// @brief Size of signature in bytes.
    constexpr size_t signature_size = 24;

    /// @brief Signature.
    using Signature = std::array<uint8_t, signature_size>;

    enum evmc_access_status : uint8_t {
        /// @brief The entry hasn't been accessed before – it's the first access.
        EVMC_ACCESS_COLD = 0,

        /// @brief The entry is already in accessed_addresses or accessed_storage_keys.
        EVMC_ACCESS_WARM = 1
    };

    /// @brief Access status.
    using AccessStatus = evmc_access_status;
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BASE_HPP_
