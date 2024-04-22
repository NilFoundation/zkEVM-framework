#include "zkevm_framework/data_types/base.hpp"

#include <iostream>
#include <optional>

namespace data_types {
    std::optional<bytes> read_bytes(std::istream& src) {
        src.seekg(0, std::ios::end);
        bytes result(src.tellg());
        src.seekg(0, std::ios::beg);
        src.read(reinterpret_cast<char*>(result.data()), result.size());
        if (src.fail()) {
            return std::nullopt;
        }
        return result;
    }

    int write_bytes(const bytes& src, std::ostream& out) {
        int cur_pos = static_cast<int>(out.tellp());
        out.write(reinterpret_cast<const char*>(src.data()), src.size());
        if (out.fail()) {
            return -1;
        }
        return static_cast<int>(out.tellp()) - cur_pos;
    }
}  // namespace data_types
