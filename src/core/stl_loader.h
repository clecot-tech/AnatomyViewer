#pragma once

#include <array>
#include <cstddef>
#include <filesystem>
#include <span>
#include <variant>
#include <vector>
#include <cstdint>

namespace anatomy::core {

static constexpr std::size_t kHeaderSize = 80;
static constexpr std::size_t kCountSize = 4;
static constexpr std::size_t kTriangleSize = 50; // 12 + 36 + 2

struct Vec3
{
    float x{};
    float y{};
    float z{};
};

struct Triangle
{
    Vec3 normal; 
    std::array<Vec3, 3> vertices;
    std::uint16_t attribute;
};

struct Mesh
{
    std::vector<Triangle> triangles;
};

enum class StlError
{
    FileTooSmall,   // fewer than 84 bytes (header + triangle count)
    SizeMismatch,   // size differs from 84 + 50 * triangle count
    EmptyMesh,      // triangle count is zero
    NonFiniteValue, // NaN or infinity in a normal or a vertex
    FileNotReadable // I/O error (file loading only)
};

using StlResult = std::variant<Mesh, StlError>;

/// Parses a binary STL from memory.
StlResult parseBinaryStl(std::span<const std::byte> data);

/// Reads a file, then parses it as a binary STL.
StlResult loadBinaryStlFile(const std::filesystem::path& path);

}