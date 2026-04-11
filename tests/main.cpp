#include <gtest/gtest.h>

extern "C" {
#include "lib/kmemory.h"
#include "lib/kstring.h"
}

TEST(KStringTests, ReturnsCorrectLength) {
    EXPECT_EQ(kstrlen(""), 0U);
    EXPECT_EQ(kstrlen("arkoi"), 5U);
}

TEST(KMemoryTests, KMemsetFillsBuffer) {
    unsigned char buffer[8] = {};

    kmemset(buffer, 0xAB, sizeof(buffer));

    for (unsigned char value : buffer) {
        EXPECT_EQ(value, 0xAB);
    }
}

TEST(KMemoryTests, KMemcpyCopiesBuffer) {
    unsigned char source[6] = { 1, 2, 3, 4, 5, 6 };
    unsigned char destination[6] = {};

    kmemcpy(destination, source, sizeof(source));

    for (int index = 0; index < 6; ++index) {
        EXPECT_EQ(destination[index], source[index]);
    }
}

TEST(KMemoryTests, AlignHelpersWork) {
    EXPECT_EQ(kalign_up(0x1003U, 0x1000U), 0x2000U);
    EXPECT_EQ(kalign_down(0x1FFFU, 0x1000U), 0x1000U);
    EXPECT_EQ(kalign_up(0x2000U, 0x1000U), 0x2000U);
}
