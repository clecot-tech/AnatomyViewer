#include <gtest/gtest.h>

#include "core/version.h"

TEST(Req008, ReturnsCurrentVersion)
{
	EXPECT_EQ(anatomy::core::version(), "0.1.0");
}

TEST(Req008, IsNotEmpty)
{
	EXPECT_FALSE(anatomy::core::version().empty());
}