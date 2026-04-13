#include <cstdint>

#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "lib/kstdio.h"
#ifdef __cplusplus
}
#endif

// This is only needed because kstdio.c includes vga.h, which has a dependency on the vga_write_string function.
extern "C" void vga_write_string([[maybe_unused]] const char* data) {
    // Do nothing, we don't want to write to the screen during tests.
}

TEST(KStdioTests, SnprintfFormatsSupportedSpecifiers) {
    char buffer[128] = {};

    const uint32_t written = ksnprintf(buffer, sizeof(buffer), "d=%d x=%x s=%s c=%c p=%%", -42, 0x1A2BU, "ok", 'Z');

    EXPECT_EQ(written, 26U);
    EXPECT_STREQ(buffer, "d=-42 x=1a2b s=ok c=Z p=%");
}

TEST(KStdioTests, SnprintfUnknownSpecifierIsLiteralized) {
    char buffer[32] = {};

    const uint32_t written = ksnprintf(buffer, sizeof(buffer), "A%qB");

    EXPECT_EQ(written, 5U);
    EXPECT_STREQ(buffer, "A%qB");
}

TEST(KStdioTests, SnprintfNullStringUsesPlaceholder) {
    char buffer[64] = {};

    const uint32_t written = ksnprintf(buffer, sizeof(buffer), "msg=%s", (char*)NULL);

    EXPECT_EQ(written, 9U);
    EXPECT_STREQ(buffer, "msg=NULL");
}

TEST(KStdioTests, SnprintfStopsBeforeOverflow) {
    char buffer[3] = {};

    const uint32_t written = ksnprintf(buffer, sizeof(buffer), "abc");

    EXPECT_EQ(written, 3U);
    EXPECT_STREQ(buffer, "ab");
}

TEST(KStdioTests, SnprintfSupportsUnsignedAndSignedAlias) {
    char buffer[128] = {};

    const uint32_t written = ksnprintf(buffer, sizeof(buffer), "u=%u i=%i", 4294967295U, -17);

    EXPECT_EQ(written, 19U);
    EXPECT_STREQ(buffer, "u=4294967295 i=-17");
}

TEST(KStdioTests, SnprintfSupportsUpperHexAndPointer) {
    char buffer[128] = {};

    const uint32_t written = ksnprintf(buffer, sizeof(buffer), "X=%#X P=%p", 0xABCDU, nullptr);

    EXPECT_EQ(written, 22U);
    EXPECT_STREQ(buffer, "X=0XABCD P=0x00000000");
}

TEST(KStdioTests, SnprintfHandlesInt32Min) {
    char buffer[32] = {};

    const uint32_t written = ksnprintf(buffer, sizeof(buffer), "%d", INT32_MIN);

    EXPECT_EQ(written, 12U);
    EXPECT_STREQ(buffer, "-2147483648");
}
