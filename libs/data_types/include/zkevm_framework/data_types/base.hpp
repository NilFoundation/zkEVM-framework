#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BASE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BASE_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

namespace data_types {
    /// @brief Vector of bytes.
    using bytes = std::vector<std::byte>;

    /**
     * @brief Read bytes from input char stream.
     *
     * @return bytes read. If I/O error occured, returns `std::nullopt`.
     */
    std::optional<bytes> read_bytes(std::istream& src);

    /**
     * @brief Write bytes to output char stream.
     *
     * @return number of bytes written. If I/O error occured, returns -1.
     */
    int write_bytes(const bytes& src, std::ostream& out);

    /// @brief Size of account address in bytes.
    constexpr size_t address_size = 20;

    /// @brief Account address.
    using Address = std::array<uint8_t, address_size>;

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

    /// @brief SSZ merkalization hash.
    using SSZHash = std::array<std::byte, 32>;

    /// @brief Hash.
    using Hash = uint64_t;  // TODO: define

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
