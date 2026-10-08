#include "core/stl_loader.h"
#include <fstream>
#include <optional>   // for std::optional
#include <cstring>    // for std::memcpy

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
        if (data.size() < kHeaderSize + kCountSize)
        {
            return StlError::FileTooSmall;
        }

        const std::byte* p = data.data() + kHeaderSize;
        uint32_t count = 0;
        std::memcpy(&count, p, sizeof(count));

        if (count == 0)
        {
            return StlError::EmptyMesh;
        }

        std::size_t expected =
            kHeaderSize + kCountSize + static_cast<std::size_t>(count) * kTriangleSize;
        if (data.size() != expected)
        {
            return StlError::SizeMismatch;
        }

        Mesh mesh;
        mesh.triangles.reserve(count);

        p += kCountSize;
        for (uint32_t i = 0; i < count; ++i) {
            Triangle t;

            auto normal = readVec3(p);
            auto v0 = readVec3(p + 12);
            auto v1 = readVec3(p + 24);
            auto v2 = readVec3(p + 36);

            if (!normal || !v0 || !v1 || !v2)
                return StlError::NonFiniteValue;

            uint16_t attr;
            std::memcpy(&attr, p + 48, sizeof(uint16_t));

            mesh.triangles.push_back(Triangle{*normal, {{*v0, *v1, *v2}}, attr});
            p += kTriangleSize;
        }
        return mesh;

	}

	/// Reads a file, then parses it as a binary STL.
	StlResult loadBinaryStlFile(const std::filesystem::path& path)
	{
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return StlError::FileNotReadable;
        }

        auto size = file.tellg();
        std::vector<std::byte> buffer(static_cast<std::size_t>(size));
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        {
            return StlError::FileNotReadable;
        }

        return parseBinaryStl(buffer);
	}

}