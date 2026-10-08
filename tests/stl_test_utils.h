#pragma once

#include "core/stl_loader.h"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <span>
#include <type_traits>
#include <variant>
#include <vector>

static_assert(std::endian::native == std::endian::little,
              "These test helpers assume a little-endian host, like the STL format.");

namespace anatomy::test {

/// Appends the raw bytes of a plain value (integer, float...) at the end of `out`.
template <typename T> void appendValue(std::vector<std::byte>& out, const T& value)
{
    static_assert(std::is_trivially_copyable_v<T>, "Only plain data can be copied as raw bytes.");
    const auto bytes = std::as_bytes(std::span{&value, 1});
    out.insert(out.end(), bytes.begin(), bytes.end());
}

inline void appendVec3(std::vector<std::byte>& out, const core::Vec3& v)
{
    appendValue(out, v.x);
    appendValue(out, v.y);
    appendValue(out, v.z);
}

/// Builds the bytes of a binary STL file. `declaredCount` is written in the file as is:
/// pass a value different from triangles.size() to simulate a corrupted file.
inline std::vector<std::byte> makeBinaryStl(const std::vector<core::Triangle>& triangles,
                                            std::uint32_t declaredCount,
                                            std::uint16_t attributeByteCount = 0)
{
    std::vector<std::byte> bytes(80, std::byte{0}); // header: content is meaningless
    appendValue(bytes, declaredCount);
    for (const auto& triangle : triangles) {
        appendVec3(bytes, triangle.normal);
        for (const auto& vertex : triangle.vertices) {
            appendVec3(bytes, vertex);
        }
        appendValue(bytes, attributeByteCount);
    }
    return bytes;
}

/// Valid file: the declared count matches the triangles provided.
inline std::vector<std::byte> makeBinaryStl(const std::vector<core::Triangle>& triangles)
{
    return makeBinaryStl(triangles, static_cast<std::uint32_t>(triangles.size()));
}

/// Checks that a result holds the expected error.
inline void expectError(const core::StlResult& result, core::StlError expected)
{
    ASSERT_TRUE(std::holds_alternative<core::StlError>(result));
    EXPECT_EQ(std::get<core::StlError>(result), expected);
}

} // namespace anatomy::test