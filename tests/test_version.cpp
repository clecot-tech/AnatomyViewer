#include <gtest/gtest.h>

#include "core/version.h"

TEST(Version, ReturnsCurrentVersion)
{
    EXPECT_EQ(anatomy::core::version(), "0.1.0");
}

TEST(Version, IsNotEmpty)
{
    EXPECT_FALSE(anatomy::core::version().empty());
}