# Test Cases
This folder contains test cases that can be used in the Tester program

Every *.utf8.txt file is encoded in UTF-8, without a BOM.
Every *.utf16.txt file is encoded in UTF-16LE, without a BOM.

## Two-Way (`two-way`)
The `two-way` folder contains all-valid text that can serve as generic non-edge cases.
Files in this folder should be the same if converted either way (UTF-8 to UTF-16 or UTF-16 to UTF-8).

* `all`
    * Every Unicode codepoint from U+0000 to U+10FFFF, excluding UTF-16 surrogates and [noncharacters](http://www.unicode.org/faq/private_use.html#noncharacters)
* `bible`
    * The King James Bible. Four megabytes of text, intended as a pseudo-"stress test"
* `chinese`
    * A poem in traditional and simplified Chinese
* `emoji`
    * A large collection of emoji
* `lorem_ipsum`
    * Five paragraphs of Lorem Ipsum

## One-Way (`utf8-to-utf16`, `utf16-to-utf8`)
The `utf8-to-utf16` and `utf16-to-utf8` folders contain specific invalid encodings that can be
used to test the handling of edge cases in the conversions.

These files can only be used one way, because the invalid encoding will be replaced by U+FFFD,
losing information about the original invalid bytes.

### `utf8-to-utf16`
* `overlong`
    * Overlong encoding of ASCII `A`, spanning four bytes instead of one
* `truncated`
    * Truncated encoding of the emoji 👍, with the last byte missing
* `rogue`
    * A rogue continuation byte (`10xxxxxx`) without a leading byte
* `surrogate`
    * The UTF-16 surrogate 0xD801 encoded in UTF-8

### `utf16-to-utf8`
* `unmatched_high`
    * Unmatched high surrogate for 👍
* `unmatched_low`
    * Unmatched low surrogate for 👍
