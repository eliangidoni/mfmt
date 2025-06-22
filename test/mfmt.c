#include <mfmt.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void
test_mfmt_print_basic(void) {
    char buf[100];
    int len;

    // Test basic string
    len = mfmt_print(buf, sizeof(buf), "Hello, World!");
    assert(strcmp(buf, "Hello, World!") == 0);
    assert(len == 13);

    // Test integer formatting
    len = mfmt_print(buf, sizeof(buf), "Value: %d", 42);
    assert(strcmp(buf, "Value: 42") == 0);
    assert(len == 9);

    // Test negative integer
    len = mfmt_print(buf, sizeof(buf), "Negative: %d", -123);
    assert(strcmp(buf, "Negative: -123") == 0);
    assert(len == 14);

    // Test unsigned integer
    len = mfmt_print(buf, sizeof(buf), "Unsigned: %u", 456u);
    assert(strcmp(buf, "Unsigned: 456") == 0);
    assert(len == 13);
}

static void
test_mfmt_print_hex(void) {
    char buf[100];
    int len;

    // Test lowercase hex
    len = mfmt_print(buf, sizeof(buf), "Hex: %x", 255);
    assert(strcmp(buf, "Hex: ff") == 0);
    assert(len == 7);

    // Test uppercase hex
    len = mfmt_print(buf, sizeof(buf), "HEX: %X", 255);
    assert(strcmp(buf, "HEX: FF") == 0);
    assert(len == 7);

    // Test hex with larger value
    len = mfmt_print(buf, sizeof(buf), "Large: %x", 0xDEADBEEF);
    assert(strcmp(buf, "Large: deadbeef") == 0);
    assert(len == 15);
}

static void
test_mfmt_print_char_string(void) {
    char buf[100];
    int len;

    // Test character
    len = mfmt_print(buf, sizeof(buf), "Char: %c", 'A');
    assert(strcmp(buf, "Char: A") == 0);
    assert(len == 7);

    // Test string
    len = mfmt_print(buf, sizeof(buf), "String: %s", "test");
    assert(strcmp(buf, "String: test") == 0);
    assert(len == 12);

    // Test empty string
    len = mfmt_print(buf, sizeof(buf), "Empty: %s", "");
    assert(strcmp(buf, "Empty: ") == 0);
    assert(len == 7);
}

static void
test_mfmt_print_width_padding(void) {
    char buf[100];
    int len;

    // Test width with space padding
    len = mfmt_print(buf, sizeof(buf), "Padded: %5d", 42);
    assert(strcmp(buf, "Padded:    42") == 0);
    assert(len == 13);

    // Test width with zero padding
    len = mfmt_print(buf, sizeof(buf), "Zero: %05d", 42);
    assert(strcmp(buf, "Zero: 00042") == 0);
    assert(len == 11);

    // Test string width
    len = mfmt_print(buf, sizeof(buf), "String: %10s", "hi");
    assert(strcmp(buf, "String:         hi") == 0);
    assert(len == 18);
}

static void
test_mfmt_print_percent(void) {
    char buf[100];
    int len;

    // Test literal percent
    len = mfmt_print(buf, sizeof(buf), "Percent: %%");
    assert(strcmp(buf, "Percent: %") == 0);
    assert(len == 10);

    // Test percent with other formatting
    len = mfmt_print(buf, sizeof(buf), "Mix: %d%% done", 50);
    assert(strcmp(buf, "Mix: 50% done") == 0);
    assert(len == 13);
}

static void
test_mfmt_print_pointer(void) {
    char buf[100];
    int len;
    int dummy = 42;

    // Test pointer formatting (just check it doesn't crash and has reasonable length)
    len = mfmt_print(buf, sizeof(buf), "Ptr: %p", &dummy);
    assert(len > 5); // Should be at least "Ptr: " + some hex digits
    assert(strncmp(buf, "Ptr: ", 5) == 0);
}

static void
test_mfmt_print_combinations(void) {
    char buf[200];
    int len;

    // Test multiple format specifiers (now working after fixes!)
    len = mfmt_print(buf, sizeof(buf), "Dec: %d, Hex: %x, Str: %s, Char: %c",
                     42, 255, "test", 'A');
    assert(strcmp(buf, "Dec: 42, Hex: ff, Str: test, Char: A") == 0);
    assert(len == 36);

    // Test mixed width formatting
    len = mfmt_print(buf, sizeof(buf), "%05d:%02x:%10s", 7, 15, "OK");
    assert(strcmp(buf, "00007:0f:        OK") == 0);

    // Test multiple strings
    len = mfmt_print(buf, sizeof(buf), "First: %s, Second: %s", "hello", "world");
    assert(strcmp(buf, "First: hello, Second: world") == 0);

    // Test many arguments
    len = mfmt_print(buf, sizeof(buf), "%d-%d-%d-%d-%d", 1, 2, 3, 4, 5);
    assert(strcmp(buf, "1-2-3-4-5") == 0);
}

static void
test_mfmt_scan_basic(void) {
    int val;
    int ret;

    // Test basic integer scanning
    ret = mfmt_scan("42", "%d", &val);
    assert(ret == 1);
    assert(val == 42);

    // Test negative integer
    ret = mfmt_scan("-123", "%d", &val);
    assert(ret == 1);
    assert(val == -123);

    // Test with leading spaces
    ret = mfmt_scan("   456", "%d", &val);
    assert(ret == 1);
    assert(val == 456);
}

static void
test_mfmt_scan_unsigned_hex(void) {
    unsigned int val;
    int ret;

    // Test unsigned integer
    ret = mfmt_scan("123", "%u", &val);
    assert(ret == 1);
    assert(val == 123);

    // Test hex scanning (lowercase)
    ret = mfmt_scan("ff", "%x", &val);
    assert(ret == 1);
    assert(val == 255);

    // Test hex scanning (uppercase)
    ret = mfmt_scan("FF", "%X", &val);
    assert(ret == 1);
    assert(val == 255);

    // Test hex with 0x prefix
    ret = mfmt_scan("0xFF", "%x", &val);
    assert(ret == 1);
    assert(val == 255);
}

static void
test_mfmt_scan_char_string(void) {
    char ch;
    char str[50];
    int ret;

    // Test character scanning
    ret = mfmt_scan("A", "%c", &ch);
    assert(ret == 1);
    assert(ch == 'A');

    // Test string scanning
    ret = mfmt_scan("hello", "%s", str);
    assert(ret == 1);
    assert(strcmp(str, "hello") == 0);

    // Test string with spaces (should stop at first space)
    ret = mfmt_scan("hello world", "%s", str);
    assert(ret == 1);
    assert(strcmp(str, "hello") == 0);
}

static void
test_mfmt_scan_width(void) {
    char str[50];
    char ch[10];
    int ret;

    // Test string with width limit
    ret = mfmt_scan("hello", "%3s", str);
    assert(ret == 1);
    assert(strcmp(str, "hel") == 0);

    // Test character with width
    ret = mfmt_scan("abc", "%2c", ch);
    assert(ret == 1);
    assert(ch[0] == 'a');
    assert(ch[1] == 'b');

    // Single character test
    ret = mfmt_scan("x", "%c", ch);
    assert(ret == 1);
    assert(ch[0] == 'x');
}

static void
test_mfmt_scan_simple(void) {
    int num1, num2, num3;
    char str[20];
    int ret;

    // Test simple cases that work
    ret = mfmt_scan("42", "%d", &num1);
    assert(ret == 1);
    assert(num1 == 42);

    ret = mfmt_scan("hello", "%s", str);
    assert(ret == 1);
    assert(strcmp(str, "hello") == 0);

    // Test with literal characters
    ret = mfmt_scan("Value: 123", "Value: %d", &num1);
    assert(ret == 1);
    assert(num1 == 123);

    // Test multiple integers (now working!)
    ret = mfmt_scan("42 100 -50", "%d %d %d", &num1, &num2, &num3);
    assert(ret == 3);
    assert(num1 == 42);
    assert(num2 == 100);
    assert(num3 == -50);
}

static void
test_mfmt_scan_percent(void) {
    int val;
    int ret;

    // Test literal percent
    ret = mfmt_scan("100%", "%d%%", &val);
    assert(ret == 1);
    assert(val == 100);
}

static void
test_mfmt_scan_edge_cases(void) {
    int val;
    unsigned int uval;
    char str[20];
    int ret;

    // Test leading zeros
    ret = mfmt_scan("007", "%d", &val);
    assert(ret == 1);
    assert(val == 7);

    // Test hex with mixed case
    ret = mfmt_scan("aBc", "%x", &uval);
    assert(ret == 1);
    assert(uval == 0xABC);

    // Test string with no spaces
    ret = mfmt_scan("test123", "%s", str);
    assert(ret == 1);
    assert(strcmp(str, "test123") == 0);
}

static void
test_edge_cases(void) {
    char buf[10];
    int len;
    int val;
    int ret;

    // Test buffer size limits
    len = mfmt_print(buf, sizeof(buf), "This is a very long string");
    assert(len <= (int)sizeof(buf) - 1); // Should fit in buffer minus null terminator
    assert(buf[sizeof(buf) - 1] == '\0'); // Should be null terminated

    // Test zero buffer size
    len = mfmt_print(buf, 0, "test");
    assert(len == 0);

    // Test scanning invalid input
    ret = mfmt_scan("abc", "%d", &val);
    assert(ret == 0); // Should fail to parse

    // Test scanning empty string
    ret = mfmt_scan("", "%d", &val);
    assert(ret == 0);

    // Test single character buffer
    char small[2];
    len = mfmt_print(small, sizeof(small), "Hi");
    assert(len == 1); // Should only fit 'H' + null terminator
    assert(small[0] == 'H');
    assert(small[1] == '\0');

    // Test empty string handling (fixed!)
    char larger[50];
    len = mfmt_print(larger, sizeof(larger), "Before:|%s|After", "");
    assert(strcmp(larger, "Before:||After") == 0);
    assert(len == 14);
}

static void
test_mfmt_scan_combinations(void) {
    int i1, i2, i3;
    unsigned int u1, u2;
    char s1[50], s2[50], s3[50];
    char c1, c2;
    int ret;

    // Test 1: Multiple different types
    ret = mfmt_scan("123 hello 0xFF", "%d %s %x", &i1, s1, &u1);
    assert(ret == 3);
    assert(i1 == 123);
    assert(strcmp(s1, "hello") == 0);
    assert(u1 == 0xFF);

    // Test 2: Multiple strings separated by delimiters
    ret = mfmt_scan("first second third", "%s %s %s", s1, s2, s3);
    assert(ret == 3);
    assert(strcmp(s1, "first") == 0);
    assert(strcmp(s2, "second") == 0);
    assert(strcmp(s3, "third") == 0);

    // Test 3: Numbers with different formats
    ret = mfmt_scan("42 99 0xAB", "%d %u %x", &i1, &u1, &u2);
    assert(ret == 3);
    assert(i1 == 42);
    assert(u1 == 99);
    assert(u2 == 0xAB);

    // Test 4: Mix of characters and numbers
    ret = mfmt_scan("A 65 B", "%c %d %c", &c1, &i1, &c2);
    assert(ret == 3);
    assert(c1 == 'A');
    assert(i1 == 65);
    assert(c2 == 'B');

    // Test 5: Complex pattern with literals
    ret = mfmt_scan("ID:12345", "ID:%d", &i1);
    assert(ret == 1);
    assert(i1 == 12345);

    // Test 6: Multiple integers with negative values
    ret = mfmt_scan("42 -100 0", "%d %d %d", &i1, &i2, &i3);
    assert(ret == 3);
    assert(i1 == 42);
    assert(i2 == -100);
    assert(i3 == 0);

    // Test 7: Hex values in different cases
    ret = mfmt_scan("abc DEF 123", "%x %X %x", &u1, &u2, &i1);
    assert(ret == 3);
    assert(u1 == 0xABC);
    assert(u2 == 0xDEF);
    assert(i1 == 0x123);

    // Test 8: String and character combinations
    ret = mfmt_scan("word X", "%s %c", s1, &c1);
    assert(ret == 2);
    assert(strcmp(s1, "word") == 0);
    assert(c1 == 'X');

    // Test 9: Leading/trailing spaces handling
    ret = mfmt_scan("  42   hello  ", "%d %s", &i1, s1);
    assert(ret == 2);
    assert(i1 == 42);
    assert(strcmp(s1, "hello") == 0);

    // Test 10: Maximum supported arguments (stress test)
    int n1, n2, n3, n4, n5;
    ret = mfmt_scan("1 2 3 4 5", "%d %d %d %d %d", &n1, &n2, &n3, &n4, &n5);
    assert(ret == 5);
    assert(n1 == 1 && n2 == 2 && n3 == 3 && n4 == 4 && n5 == 5);

    // Test 11: Width-limited scanning combinations
    ret = mfmt_scan("hello123world456", "%5s%3d%5s%3d", s1, &i1, s2, &i2);
    assert(ret == 4);
    assert(strcmp(s1, "hello") == 0);
    assert(i1 == 123);
    assert(strcmp(s2, "world") == 0);
    assert(i2 == 456);

    // Test 12: Mixed hex formats (upper and lower case)
    ret = mfmt_scan("ff FF 0x99 0XAA", "%x %X %x %X", &u1, &u2, &i1, &i2);
    assert(ret == 4);
    assert(u1 == 0xFF);
    assert(u2 == 0xFF);
    assert(i1 == 0x99);
    assert(i2 == 0xAA);

    // Test 13: Percent literal in combinations
    ret = mfmt_scan("100% complete 50", "%d%% %s %d", &i1, s1, &i2);
    assert(ret == 3);
    assert(i1 == 100);
    assert(strcmp(s1, "complete") == 0);
    assert(i2 == 50);
}

int main(void) {

    // Test mfmt_print functionality
    test_mfmt_print_basic();
    test_mfmt_print_hex();
    test_mfmt_print_char_string();
    test_mfmt_print_width_padding();
    test_mfmt_print_percent();
    test_mfmt_print_pointer();
    test_mfmt_print_combinations();

    // Test mfmt_scan functionality
    test_mfmt_scan_basic();
    test_mfmt_scan_unsigned_hex();
    test_mfmt_scan_char_string();
    test_mfmt_scan_width();
    test_mfmt_scan_simple();
    test_mfmt_scan_percent();
    test_mfmt_scan_edge_cases();
    test_mfmt_scan_combinations();

    // Test edge cases
    test_edge_cases();

    return 0;
}
