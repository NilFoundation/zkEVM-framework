#include "zkevm_framework/data_types/base.hpp"

#include <cerrno>
#include <cstring>
#include <expected>
#include <iostream>

#include "zkevm_framework/data_types/errors.hpp"

namespace data_types {
    std::expected<bytes, IOError> read_bytes(std::istream& in) noexcept {
        // disable exceptions for stream, if they are enabled
        // same current exceptions mask to restore it at the end
        std::ios_base::iostate mask = in.exceptions();
        in.exceptions(std::ios_base::goodbit);  // set mask to 0 to disable all exceptions

        // construct resulting bytes of desired size
        in.seekg(0, std::ios::end);
        // check for errors
        if (in.fail() || in.bad()) {
            in.exceptions(mask);  // restore exceptions mask
            return std::unexpected(IOError(std::strerror(errno)));
        }

        bytes result;
        try {
            result.resize(in.tellg());
        } catch (const std::exception& e) {
            return std::unexpected(IOError(e.what()));
        }

        in.seekg(0, std::ios::beg);
        // check for errors
        if (in.fail() || in.bad()) {
            in.exceptions(mask);  // restore exceptions mask
            return std::unexpected(IOError(std::strerror(errno)));
        }

        in.read(reinterpret_cast<char*>(result.data()), result.size());  // perform the read
        // check for errors
        if (in.fail() || in.bad()) {
            in.exceptions(mask);  // restore exceptions mask
            return std::unexpected(IOError(std::strerror(errno)));
        }

        in.exceptions(mask);  // restore exceptions mask
        return result;
    }

    std::expected<void, IOError> write_bytes(const bytes& src, std::ostream& out) noexcept {
        // disable exceptions for stream, if they are enabled
        // same current exceptions mask to restore it at the end
        std::ios_base::iostate mask = out.exceptions();
        out.exceptions(std::ios_base::goodbit);  // set mask to 0 to disable all exceptions

        out.write(reinterpret_cast<const char*>(src.data()), src.size());  // perform the write

        // check for errors
        if (out.fail() || out.bad()) {
            out.exceptions(mask);  // restore exceptions mask
            return std::unexpected(IOError(std::strerror(errno)));
        }

        out.exceptions(mask);  // restore exceptions mask
        return std::expected<void, IOError>();
    }
}  // namespace data_types
