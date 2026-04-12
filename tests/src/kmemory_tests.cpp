#include <cstdint>

#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "lib/kmemory.h"
#ifdef __cplusplus
}
#endif

TEST(KMemoryTests, KMemcpyInvalidArguments) {
    uint8_t buffer[4] = {};

    EXPECT_EQ(kmemcpy(nullptr, buffer, sizeof(buffer)), nullptr);
    EXPECT_EQ(kmemcpy(buffer, nullptr, sizeof(buffer)), nullptr);
    EXPECT_EQ(kmemcpy(buffer, buffer, 0), buffer);
}

TEST(KMemoryTests, KMemcpyCopiesBytes) {
    uint8_t source[6] = { 1, 2, 3, 4, 5, 6 };
    uint8_t destination[6] = {};

    EXPECT_EQ(kmemcpy(destination, source, sizeof(source)), destination);

    for (int index = 0; index < 6; ++index) {
        EXPECT_EQ(destination[index], source[index]);
    }
}

TEST(KMemoryTests, KMemcpyHandlesZeroLength) {
    uint8_t source[4] = { 9, 8, 7, 6 };
    uint8_t destination[4] = { 1, 2, 3, 4 };

    EXPECT_EQ(kmemcpy(destination, source, 0), destination);

    for (int index = 0; index < 4; ++index) {
        EXPECT_EQ(destination[index], static_cast<uint8_t>(index + 1));
    }
}

TEST(KMemoryTests, KMemcpyCopiesSubrange) {
    uint8_t source[6] = { 1, 2, 3, 4, 5, 6 };
    uint8_t destination[6] = {};

    EXPECT_EQ(kmemcpy(destination + 2, source + 2, 3), destination + 2);

    EXPECT_EQ(destination[0], 0U);
    EXPECT_EQ(destination[1], 0U);
    EXPECT_EQ(destination[2], 3U);
    EXPECT_EQ(destination[3], 4U);
    EXPECT_EQ(destination[4], 5U);
    EXPECT_EQ(destination[5], 0U);
}

TEST(KMemoryTests, KMemsetInvalidArguments) {
    uint8_t buffer[4] = {};

    EXPECT_EQ(kmemset(nullptr, 0xFF, sizeof(buffer)), nullptr);
    EXPECT_EQ(kmemset(buffer, 0xFF, 0), buffer);
}

TEST(KMemoryTests, KMemsetFillsBuffer) {
    uint8_t buffer[8] = {};

    kmemset(buffer, 0xAB, sizeof(buffer));

    for (uint8_t value : buffer) {
        EXPECT_EQ(value, 0xAB);
    }
}

TEST(KMemoryTests, KMemsetHandlesZeroLength) {
    uint8_t buffer[4] = { 1, 2, 3, 4 };

    kmemset(buffer, 0xCD, 0);

    for (int index = 0; index < 4; ++index) {
        EXPECT_EQ(buffer[index], static_cast<uint8_t>(index + 1));
    }
}

TEST(KMemoryTests, AlignHelpersWork) {
    EXPECT_EQ(kalign_up(0x1003U, 0x1000U), 0x2000U);
    EXPECT_EQ(kalign_down(0x1003U, 0x1000U), 0x1000U);

    EXPECT_EQ(kalign_up(0x1FFFU, 0x1000U), 0x2000U);
    EXPECT_EQ(kalign_down(0x1FFFU, 0x1000U), 0x1000U);

    EXPECT_EQ(kalign_up(0x2000U, 0x1000U), 0x2000U);
    EXPECT_EQ(kalign_down(0x2000U, 0x1000U), 0x2000U);
}
