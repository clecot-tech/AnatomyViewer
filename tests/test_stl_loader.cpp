#include "core/stl_loader.h"
#include "stl_test_utils.h"

#include <gtest/gtest.h>
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
                    {{Vec3{0.0f, 0.0f, 0.0f}, Vec3{1.0f, 0.0f, 0.0f}, Vec3{0.0f, 1.0f, 0.0f}}}};
}

Triangle makeTriangleWithNaN()
{
    return Triangle{Vec3{0.0f, 0.0f, 1.0f},
        {{Vec3{0.0f, 0.0f, 0.0f}, Vec3{1.0f, 0.0f, std::numeric_limits<float>::quiet_NaN()},
          Vec3{0.0f, 1.0f, 0.0f}}}};
}

}

TEST(Req001, ParsesSingleTriangle)
{
    const auto result = parseBinaryStl(makeBinaryStl({makeTriangle()}));

    ASSERT_TRUE(std::holds_alternative<Mesh>(result));
    const auto& mesh = std::get<Mesh>(result);
    ASSERT_EQ(mesh.triangles.size(), 1u);
    EXPECT_EQ(mesh.triangles[0].vertices[1].x, 1.0f);
}

TEST(Req002, RejectsTooSmallBuffer)
{
    const std::vector<std::byte> tooSmall(4);
    expectError(parseBinaryStl(tooSmall), StlError::FileTooSmall);
}

TEST(Req002, RejectsTruncatedData)
{
    // The file claims 2 triangles but only contains 1.
    const auto bytes = makeBinaryStl({makeTriangle()}, 2);
    expectError(parseBinaryStl(bytes), StlError::SizeMismatch);
}

TEST(Req002, RejectsEmptyMesh)
{
    // The file claims 0 triangle.
    const auto bytes = makeBinaryStl({makeTriangle()}, 0);
    expectError(parseBinaryStl(bytes), StlError::EmptyMesh);
}

TEST(Req002, RejectsCorruptedData)
{
    const auto bytes = makeBinaryStl({makeTriangleWithNaN()}, 1);
    expectError(parseBinaryStl(bytes), StlError::NonFiniteValue);
}