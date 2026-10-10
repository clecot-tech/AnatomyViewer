#include "core/stl_loader.h"

#include <cmath>   // for std::isfinite
#include <cstring> // for std::memcpy
#include <fstream>
#include <optional> // for std::optional

namespace anatomy::core {

static std::optional<Vec3> readVec3(const std::byte* p)
{
    Vec3 v;
    std::memcpy(&v.x, p, sizeof(float));
    std::memcpy(&v.y, p + 4, sizeof(float));
    std::memcpy(&v.z, p + 8, sizeof(float));
    if (!std::isfinite(v.x) || !std::isfinite(v.y) || !std::isfinite(v.z))
        return std::nullopt;
    return v;
}

/// Parses a binary STL from memory.
StlResult parseBinaryStl(std::span<const std::byte> data)
{
    if (data.size() < kHeaderSize + kCountSize) {
        return StlError::FileTooSmall;
    }

    const std::byte* p = data.data() + kHeaderSize;
    std::uint32_t count = 0;
    std::memcpy(&count, p, sizeof(count));

    const std::uint64_t expected = kHeaderSize + kCountSize + std::uint64_t{count} * kTriangleSize;
    if (data.size() != expected) {
        return StlError::SizeMismatch;
    }

    if (count == 0) {
        return StlError::EmptyMesh;
    }

    Mesh mesh;
    mesh.triangles.reserve(count);

    p += kCountSize;
    for (std::uint32_t i = 0; i < count; ++i) {

        auto normal = readVec3(p);
        auto v0 = readVec3(p + 12);
        auto v1 = readVec3(p + 24);
        auto v2 = readVec3(p + 36);

        if (!normal || !v0 || !v1 || !v2)
            return StlError::NonFiniteValue;

        std::uint16_t attr = 0;
        std::memcpy(&attr, p + 48, sizeof(uint16_t));

        mesh.triangles.push_back(Triangle{*normal, {{*v0, *v1, *v2}}, attr});
        p += kTriangleSize;
    }
    return mesh;
}

/// Reads a file, then parses it as a binary STL.
StlResult loadBinaryStlFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate); // ate : at the end
    if (!file) {
        return StlError::FileNotReadable;
    }

    const std::streamsize size =
        file.tellg(); // pointer at the end of file : gives size of the file
    if (size < 0) {
        return StlError::FileNotReadable;
    }
    file.seekg(0); // Back to the start before reading

    std::vector<std::byte> buffer(static_cast<std::size_t>(size));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return StlError::FileNotReadable;
    }

    return parseBinaryStl(buffer);
}

} // namespace anatomy::core