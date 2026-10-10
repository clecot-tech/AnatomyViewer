#include "core/stl_loader.h"
#include "stl_test_utils.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <limits>
#include <variant>
#include <vector>

using anatomy::core::Mesh;
using anatomy::core::parseBinaryStl;
using anatomy::core::StlError;
using anatomy::core::Triangle;
using anatomy::core::Vec3;
using anatomy::test::expectError;
using anatomy::test::makeBinaryStl;

namespace {

Triangle makeTriangle()
{
    return Triangle{Vec3{0.0f, 0.0f, 1.0f},
                    {{Vec3{0.0f, 0.0f, 0.0f}, Vec3{1.0f, 0.0f, 0.0f}, Vec3{0.0f, 1.0f, 0.0f}}},
                    0u};
}

Triangle makeTriangleWithNaN()
{
    return Triangle{
        Vec3{0.0f, 0.0f, 1.0f},
        {{Vec3{0.0f, 0.0f, 0.0f}, Vec3{1.0f, 0.0f, std::numeric_limits<float>::quiet_NaN()},
          Vec3{0.0f, 1.0f, 0.0f}}},
            0u};
}

std::vector<Triangle> makeTriangles(std::uint32_t count)
{ 
    std::vector<Triangle> triangles = {};
    for (uint32_t i = 0; i < count; i++)
    {
        triangles.push_back(makeTriangle());
    }
    return triangles;
}

} // namespace

TEST(Req001, ParsesSingleTriangle)
{
    const auto result = parseBinaryStl(makeBinaryStl({makeTriangle()}));

    ASSERT_TRUE(std::holds_alternative<Mesh>(result));
    const auto& mesh = std::get<Mesh>(result);
    ASSERT_EQ(mesh.triangles.size(), 1u);
    EXPECT_EQ(mesh.triangles[0].vertices[1].x, 1.0f);
}

TEST(Req001, ParsesSeveralTriangles)
{
    const auto result = parseBinaryStl(makeBinaryStl({makeTriangles(500u)}));

    ASSERT_TRUE(std::holds_alternative<Mesh>(result));
    const auto& mesh = std::get<Mesh>(result);
    ASSERT_EQ(mesh.triangles.size(), 500u);
    EXPECT_EQ(mesh.triangles[0].vertices[1].x, 1.0f);
}

TEST(Req001, LoadsValidFile)
{
    // Create a valid test file to load
    const auto path = std::filesystem::temp_directory_path() / "anatomy_viewer_valid.stl";
    const auto bytes = makeBinaryStl({makeTriangle()});
    {
        std::ofstream out(path, std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytes.data()),
                  static_cast<std::streamsize>(bytes.size()));
    }

    const auto result = anatomy::core::loadBinaryStlFile(path);
    std::filesystem::remove(path); // Delete test file

    ASSERT_TRUE(std::holds_alternative<Mesh>(result));
    EXPECT_EQ(std::get<Mesh>(result).triangles.size(), 1u);
}

TEST(Req001, IgnoresHeaderContent)
{
    std::vector<std::byte> binary = makeBinaryStl({makeTriangle()});
    binary[0] = std::byte('s');
    binary[1] = std::byte('o');
    binary[2] = std::byte('l');
    binary[3] = std::byte('i');
    binary[4] = std::byte('d');
    const auto result = parseBinaryStl({binary});

    ASSERT_TRUE(std::holds_alternative<Mesh>(result));
    const auto& mesh = std::get<Mesh>(result);
    ASSERT_EQ(mesh.triangles.size(), 1u);
    EXPECT_EQ(mesh.triangles[0].normal.z, 1u);
}

// To ensure parsing works whatever attibute value is written. 
TEST(Req001, IgnoresAttributeByteCount)
{
    const auto result = parseBinaryStl({makeBinaryStl({makeTriangle()}, 1, 0xABCD)});

    ASSERT_TRUE(std::holds_alternative<Mesh>(result));
    const auto& mesh = std::get<Mesh>(result);
    ASSERT_EQ(mesh.triangles.size(), 1u);
    EXPECT_EQ(mesh.triangles[0].attribute, 0xABCD);
}


TEST(Req002, RejectsMissingFile)
{
    expectError(anatomy::core::loadBinaryStlFile("this_file_does_not_exist.stl"),
                StlError::FileNotReadable);
}

TEST(Req002, RejectsTooSmallBuffer)
{
    const std::vector<std::byte> tooSmall_lowlimit(0);
    expectError(parseBinaryStl(tooSmall_lowlimit), StlError::FileTooSmall);

    const std::vector<std::byte> tooSmall(4);
    expectError(parseBinaryStl(tooSmall), StlError::FileTooSmall);

    const std::vector<std::byte> tooSmall_highlimit(83);
    expectError(parseBinaryStl(tooSmall_highlimit), StlError::FileTooSmall);
}

TEST(Req002, RejectsTruncatedData)
{
    // The file claims 2 triangles but only contains 1.
    const auto bytes = makeBinaryStl({makeTriangle()}, 2);
    expectError(parseBinaryStl(bytes), StlError::SizeMismatch);
}

TEST(Req002, RejectsEmptyMesh)
{
    // Valid file with zero triangle: exactly 84 bytes.
    expectError(parseBinaryStl(makeBinaryStl({})), StlError::EmptyMesh);
}

TEST(Req002, RejectsCountZeroWithTrailingData)
{
    // The file claims 0 triangle but contains one: inconsistent size.
    expectError(parseBinaryStl(makeBinaryStl({makeTriangle()}, 0)), StlError::SizeMismatch);
}

TEST(Req002, RejectsTrailingBytes)
{
    // The file claims 1 triangle but contains two: inconsistent size.
    expectError(parseBinaryStl(makeBinaryStl({makeTriangles(2)}, 1)), StlError::SizeMismatch);
}

TEST(Req002, RejectsCorruptedData)
{
    const auto bytes = makeBinaryStl({makeTriangleWithNaN()}, 1);
    expectError(parseBinaryStl(bytes), StlError::NonFiniteValue);
}

TEST(Req002, RejectsHugeCountWithoutAllocating)
{
    // Only 84 bytes (header + count), but the count claims about 4 billion triangles.
    // The parser must compare sizes first and reject the file without allocating anything.
    const auto bytes = makeBinaryStl({}, 0xFFFFFFFF);
    expectError(parseBinaryStl(bytes), StlError::SizeMismatch);
}