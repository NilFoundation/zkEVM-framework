#include "zkevm_framework/data_types/transaction.hpp"

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    bytes Transaction::serialize() const {
        return ssz::serialize<Transaction::Serializable>(*this);
    }

    Transaction Transaction::deserialize(bytes src) {
        return ssz::deserialize<Transaction::Serializable>(src);
    }

    SSZHash Transaction::hash_tree_root() const {
        Transaction::Serializable transaction(*this);
        SSZHash result = ssz::hash_tree_root(transaction, 0);
        return result;
    }
}  // namespace data_types
