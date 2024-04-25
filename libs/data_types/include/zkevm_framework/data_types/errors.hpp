/**
 * @file errors.hpp
 * @brief This file error types.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ERRORS_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ERRORS_HPP_

#include <cstdint>
#include <ostream>
#include <string>

/**
 * @brief Get the value of std::expected or return error from function.
 * This macro is useful for error propagation.
 *
 * @note Statement expressions are used in this macro. See:
 * https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
 *
 * @example
 * std::expected<bool, Error> get_bool();
 *
 * std::expected<void, Error> do_something() {
 *     bool val = TRYX(get_bool());  // this will produce a bool value or return Error
 *                                   // from `do_something()`
 * }
 */
#define TRYX(expr)                                               \
    ({                                                           \
        auto __result = (expr);                                  \
        if (!__result) return std::unexpected(__result.error()); \
        std::move(__result.value());                             \
    })

namespace data_types {
    /// @brief I/O error. Simplifies reading/writing bytes errors.
    class IOError {
      public:
        std::string msg;

        IOError() {}

        IOError(const std::string &msg_) : msg(msg_) {}

        IOError(const char *msg_) : msg(msg_) {}
    };

    /// @brief Error which may occur during serialization or deserialization.
    class SerializationError {
      public:
        enum Type : uint8_t {
            IOErrorType,
            SSZErrorType,
        };

        Type type;
        std::string msg;

        SerializationError(Type type_, const std::string &msg_) : type(type_), msg(msg_) {}

        SerializationError(Type type_, const char *msg_) : type(type_), msg(msg_) {}

        SerializationError(Type type_) : type(type_) {}

        /**
         * @brief This constructor is helpful because it allows to propagate IOError into
         * SerializationError (e.g. with `TRYX` macro).
         */
        SerializationError(const IOError &err) : type(IOErrorType), msg(err.msg) {}
    };

    std::ostream &operator<<(std::ostream &os, const IOError &error);

    std::ostream &operator<<(std::ostream &os, const SerializationError::Type &type);

    std::ostream &operator<<(std::ostream &os, const SerializationError &error);
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ERRORS_HPP_
