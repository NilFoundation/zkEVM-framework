#include "zkevm_framework/data_types/errors.hpp"

#include <ostream>

namespace data_types {
    std::ostream &operator<<(std::ostream &os, const IOError &error) {
        os << "IOError";
        if (!error.msg.empty()) {
            os << ": " << error.msg;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, SerializationError::Type const &type) {
        switch (type) {
            case SerializationError::Type::IOErrorType: {
                os << "IOError";
            }
            case SerializationError::Type::SSZErrorType: {
                os << "SSZError";
            }
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const SerializationError &error) {
        os << error.type;
        if (!error.msg.empty()) {
            os << ": " << error.msg;
        }
        return os;
    }
}  // namespace data_types
