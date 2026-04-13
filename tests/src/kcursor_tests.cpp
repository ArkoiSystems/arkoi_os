#include <cstdint>

#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "lib/kcursor.h"
#ifdef __cplusplus
}
#endif

TEST(KCursorTests, InitRejectsInvalidArguments) {
    kcursor_t cursor = {};
    uint8_t buffer[4] = {};

    EXPECT_FALSE(kcursor_init(nullptr, buffer, sizeof(buffer)));
    EXPECT_FALSE(kcursor_init(&cursor, nullptr, sizeof(buffer)));
}

TEST(KCursorTests, WritesStayWithinBounds) {
    kcursor_t cursor = {};
    uint8_t buffer[5] = {};

    ASSERT_TRUE(kcursor_init(&cursor, buffer, 4));

    EXPECT_TRUE(kcursor_write_byte(&cursor, 'A'));
    EXPECT_TRUE(kcursor_write(&cursor, "BC", 2));
    EXPECT_TRUE(kcursor_write_byte(&cursor, 'D'));

    EXPECT_EQ(kcursor_written(&cursor), 4U);
    EXPECT_EQ(kcursor_remaining(&cursor), 0U);
    EXPECT_FALSE(kcursor_write_byte(&cursor, 'E'));

    EXPECT_EQ(buffer[0], 'A');
    EXPECT_EQ(buffer[1], 'B');
    EXPECT_EQ(buffer[2], 'C');
    EXPECT_EQ(buffer[3], 'D');
}

TEST(KCursorTests, AdvancePreventsOverflow) {
    kcursor_t cursor = {};
    uint8_t buffer[4] = {};

    ASSERT_TRUE(kcursor_init(&cursor, buffer, sizeof(buffer)));

    EXPECT_TRUE(kcursor_advance(&cursor, 3));
    EXPECT_FALSE(kcursor_advance(&cursor, 2));
    EXPECT_EQ(kcursor_written(&cursor), 3U);
    EXPECT_EQ(kcursor_remaining(&cursor), 1U);
}

TEST(KCursorTests, ZeroLengthWriteIsNoop) {
    kcursor_t cursor = {};
    uint8_t buffer[3] = {};

    ASSERT_TRUE(kcursor_init(&cursor, buffer, sizeof(buffer)));

    EXPECT_TRUE(kcursor_write(&cursor, "X", 0));
    EXPECT_EQ(kcursor_written(&cursor), 0U);
    EXPECT_EQ(kcursor_remaining(&cursor), 3U);
}
