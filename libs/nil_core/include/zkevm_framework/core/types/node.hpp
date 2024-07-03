#ifndef ZKEMV_FRAMEWORK_LIBS_NIL_CORE_INCLUDE_ZKEVM_FRAMEWORK_CORE_TYPES_NODE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_NIL_CORE_INCLUDE_ZKEVM_FRAMEWORK_CORE_TYPES_NODE_HPP_

#include <array>
#include <cstddef>

#include "ssz++.hpp"
#include "zkevm_framework/core/common.hpp"

namespace core {
    namespace types {
        constexpr std::size_t BranchesNum = 16;

        enum NodeKind : uint8_t { SszLeafNode = 0, SszExtensionNode = 1, SszBranchNode = 2 };

        struct NodeKindFlag : ssz::ssz_container {
            uint8_t m_value;

            SSZ_CONT(m_value)
        };

        struct Path : ssz::ssz_container {
            Bytes<1000> m_data;
            uint64_t m_offset;
            bool m_isLeaf;

            SSZ_CONT(m_data, m_offset, m_isLeaf)
        };

        struct LeafNode : ssz::ssz_container {
            Path m_path;
            Bytes<100000000> m_leafData;

            SSZ_CONT(m_path, m_leafData)
        };

        struct ExtensionNode : ssz::ssz_container {
            Path m_path;
            Bytes<1000> m_nextRef;

            SSZ_CONT(m_path, m_nextRef)
        };

        struct BranchNode : ssz::ssz_container {
            Path m_path;
            ssz::list<Bytes<1000>, BranchesNum> m_branches;
            Bytes<100000000> m_value;

            SSZ_CONT(m_path, m_branches, m_value)
        };

        std::vector<std::byte> getNodeData(std::vector<std::byte>& node_data) {
            std::cout << "getNodeData\n";
            NodeKindFlag kind_flag = ssz::deserialize<NodeKindFlag>(node_data);
            node_data.erase(node_data.begin());
            if (kind_flag.m_value == SszLeafNode) {
                const auto node = ssz::deserialize<LeafNode>(node_data);
                return node.m_leafData.data();
            } else if (kind_flag.m_value == SszBranchNode) {
                const auto node = ssz::deserialize<BranchNode>(node_data);
                return node.m_value.data();
            }
            return {};
        }
    }  // namespace types
}  // namespace core

#endif  // ZKEMV_FRAMEWORK_LIBS_NIL_CORE_INCLUDE_ZKEVM_FRAMEWORK_CORE_TYPES_NODE_HPP_
