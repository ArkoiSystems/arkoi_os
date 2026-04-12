#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "lib/kbuffer.h"
#ifdef __cplusplus
}
#endif

TEST(KBufferTests, InitStartsEmpty) {
    int storage[4] = {};

    cyclic_buffer_t buffer = {};
    EXPECT_TRUE(cyclic_buffer_init(&buffer, storage, 4, sizeof(int)));

    EXPECT_TRUE(cyclic_buffer_is_empty(&buffer));
    EXPECT_FALSE(cyclic_buffer_is_full(&buffer));
    EXPECT_EQ(cyclic_buffer_size(&buffer), 0U);
}

TEST(KBufferTests, PushPopPreservesOrder) {
    int storage[4] = {};
    int out = 0;

    cyclic_buffer_t buffer = {};
    EXPECT_TRUE(cyclic_buffer_init(&buffer, storage, 4, sizeof(int)));

    int first = 11;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &first));

    int second = 22;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &second));

    EXPECT_EQ(cyclic_buffer_size(&buffer), 2U);

    EXPECT_TRUE(cyclic_buffer_pop(&buffer, &out));
    EXPECT_EQ(out, first);

    EXPECT_TRUE(cyclic_buffer_pop(&buffer, &out));
    EXPECT_EQ(out, second);

    EXPECT_TRUE(cyclic_buffer_is_empty(&buffer));
}

TEST(KBufferTests, FullWhenContainingCapacityMinusOneElements) {
    int storage[4] = {};

    cyclic_buffer_t buffer = {};
    EXPECT_TRUE(cyclic_buffer_init(&buffer, storage, 4, sizeof(int)));

    int one = 1;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &one));

    int two = 2;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &two));

    int three = 3;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &three));

    EXPECT_TRUE(cyclic_buffer_is_full(&buffer));
    EXPECT_EQ(cyclic_buffer_size(&buffer), 3U);
}

TEST(KBufferTests, WrapAroundStillPreservesOrder) {
    int storage[4] = {};
    int out = 0;

    cyclic_buffer_t buffer = {};
    EXPECT_TRUE(cyclic_buffer_init(&buffer, storage, 4, sizeof(int)));

    int one = 1;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &one));

    int two = 2;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &two));

    int three = 3;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &three));

    EXPECT_TRUE(cyclic_buffer_pop(&buffer, &out));
    EXPECT_EQ(out, one);

    int four = 4;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &four));

    EXPECT_TRUE(cyclic_buffer_is_full(&buffer));

    EXPECT_TRUE(cyclic_buffer_pop(&buffer, &out));
    EXPECT_EQ(out, two);

    EXPECT_TRUE(cyclic_buffer_pop(&buffer, &out));
    EXPECT_EQ(out, three);

    EXPECT_TRUE(cyclic_buffer_pop(&buffer, &out));
    EXPECT_EQ(out, four);

    EXPECT_TRUE(cyclic_buffer_is_empty(&buffer));
}

TEST(KBufferTests, PushWhenFullOverwritesOldestData) {
    int storage[4] = {};
    int out = 0;

    cyclic_buffer_t buffer = {};
    EXPECT_TRUE(cyclic_buffer_init(&buffer, storage, 4, sizeof(int)));

    int one = 1;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &one));

    int two = 2;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &two));

    int three = 3;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &three));

    int four = 4;
    EXPECT_TRUE(cyclic_buffer_push(&buffer, &four));

    EXPECT_EQ(cyclic_buffer_size(&buffer), 3U);

    EXPECT_TRUE(cyclic_buffer_pop(&buffer, &out));
    EXPECT_EQ(out, two);

    EXPECT_TRUE(cyclic_buffer_pop(&buffer, &out));
    EXPECT_EQ(out, three);

    EXPECT_TRUE(cyclic_buffer_pop(&buffer, &out));
    EXPECT_EQ(out, four);

    EXPECT_TRUE(cyclic_buffer_is_empty(&buffer));
}

TEST(KBufferTests, PopWhenEmptyLeavesDestinationUntouched) {
    int storage[4] = {};
    int out = 12345;

    cyclic_buffer_t buffer = {};
    EXPECT_TRUE(cyclic_buffer_init(&buffer, storage, 4, sizeof(int)));

    EXPECT_FALSE(cyclic_buffer_pop(&buffer, &out));

    EXPECT_EQ(out, 12345);
    EXPECT_TRUE(cyclic_buffer_is_empty(&buffer));
}
