#include <converter.h>
#include <stdio.h>

// The type of a single Unicode codepoint
typedef uint32_t codepoint_t;

// The last codepoint of the Basic Multilingual Plane, which is the part of Unicode that
// UTF-16 can encode without surrogates
#define BMP_END 0xFFFF

// The highest valid Unicode codepoint
#define UNICODE_MAX 0x10FFFF

// The codepoint that is used to replace invalid encodings
#define INVALID_CODEPOINT 0xFFFD

// If a character, masked with GENERIC_SURROGATE_MASK, matches this value, it is a surrogate.
#define GENERIC_SURROGATE_VALUE 0xD800
// The mask to apply to a character before testing it against GENERIC_SURROGATE_VALUE
#define GENERIC_SURROGATE_MASK 0xF800

// If a character, masked with SURROGATE_MASK, matches this value, it is a high surrogate.
#define HIGH_SURROGATE_VALUE 0xD800
// If a character, masked with SURROGATE_MASK, matches this value, it is a low surrogate.
#define LOW_SURROGATE_VALUE 0xDC00
// The mask to apply to a character before testing it against HIGH_SURROGATE_VALUE or LOW_SURROGATE_VALUE
#define SURROGATE_MASK 0xFC00

// A mask that can be applied to a surrogate to extract the codepoint value contained in it
#define SURROGATE_CODEPOINT_MASK 0x03FF
// The number of bits of SURROGATE_CODEPOINT_MASK
#define SURROGATE_CODEPOINT_BITS 10

// The highest codepoint that can be encoded with 1 byte in UTF-8
#define UTF8_1_MAX 0x7F
// The highest codepoint that can be encoded with 2 bytes in UTF-8
#define UTF8_2_MAX 0x7FF
// The highest codepoint that can be encoded with 3 bytes in UTF-8
#define UTF8_3_MAX 0xFFFF
// The highest codepoint that can be encoded with 4 bytes in UTF-8
#define UTF8_4_MAX 0x10FFFF

// If a character, masked with CONTINUATION_MASK, matches this value, it is a UTF-8 continuation byte
#define UTF8_CONTINUATION_VALUE 0x80
// The mask to a apply to a character before testing it against CONTINUATION_VALUE
#define UTF8_CONTINUATION_MASK 0xC0
// The number of bits of a codepoint that are contained in a UTF-8 continuation byte
#define UTF8_CONTINUATION_CODEPOINT_BITS 6

// Represents a UTF-8 bit pattern that can be set or verified
typedef struct
{
    // The mask that should be applied to the character before testing it
    utf8_t mask;
    // The value that the character should be tested against after applying the mask
    utf8_t value;
} utf8_pattern;

// The patterns for leading bytes of a UTF-8 codepoint encoding
// Each pattern represents the leading byte for a character encoded with N UTF-8 bytes,
// where N is the index + 1
static const utf8_pattern utf8_leading_bytes[] =
{
    { 0x80, 0x00 }, // 0xxxxxxxx
    { 0xE0, 0xC0 }, // 110xxxxxx
    { 0xF0, 0xE0 }, // 1110xxxxx
    { 0xF8, 0xF0 }  // 11110xxxx
};


// Gets a codepoint from a UTF-16 string
// utf16: The UTF-16 string
// len: The length of the UTF-16 string, in UTF-16 characters
// index: A pointer to the current index on the string. May be manipulated in case of surrogates
static codepoint_t decode_utf16(utf16_t const* utf16, size_t len, size_t* index)
{
    utf16_t high = utf16[*index];

    // BMP character
    if ((high & GENERIC_SURROGATE_MASK) != GENERIC_SURROGATE_VALUE)
        return high; 

    // Unmatched low surrogate, invalid
    if ((high & SURROGATE_MASK) != HIGH_SURROGATE_VALUE)
        return INVALID_CODEPOINT;

    // String ended with an unmatched high surrogate, invalid
    if (*index == len - 1)
        return INVALID_CODEPOINT;
    
    utf16_t low = utf16[*index + 1];

    // Unmatched high surrogate, invalid
    if ((low & SURROGATE_MASK) != LOW_SURROGATE_VALUE)
        return INVALID_CODEPOINT;

    // Two correctly matched surrogates, increase index to indicate we've consumed
    // two characters
    (*index)++;

    // The high bits of the codepoint are the value bits of the high surrogate
    // The low bits of the codepoint are the value bits of the low surrogate
    codepoint_t result = high & SURROGATE_CODEPOINT_MASK;
    result <<= SURROGATE_CODEPOINT_BITS;
    result &= low & SURROGATE_CODEPOINT_MASK;

    // Overlong encoding
    // A BMP character, which could be encoded with a single UTF-16 character, was instead
    // encoded with a surrogate pair
    // Invalid
    if (result <= BMP_END)
        return INVALID_CODEPOINT;

    // And if all else fails, it's valid
    return result;
}

// Calculates the number of UTF-8 characters it would take to encode a codepoint
// The codepoint won't be checked for validity, that should be done beforehand.
static int calculate_utf8_size(codepoint_t codepoint)
{
    // An array with the max values would be more elegant, but a bit too heavy
    // for this common function

    if (codepoint < UTF8_1_MAX)
        return 1;

    if (codepoint < UTF8_2_MAX)
        return 2;

    if (codepoint < UTF8_3_MAX)
        return 3;

    return 4;
}

// Encodes a codepoint in a UTF-8 string.
// The codepoint won't be checked for validity, that should be done beforehand.
//
// codepoint: The codepoint to be encoded.
// utf8: The UTF-8 string
// len: The length of the UTF-8 string, in UTF-8 characters
// index: The first empty index on the string.
//
// return: The number of characters written to the string.
static size_t encode_utf8(codepoint_t codepoint, utf8_t* utf8, size_t len, size_t index)
{
    int size = calculate_utf8_size(codepoint);

    // Not enough space left on the string
    if (index + size > len)
        return 0;

    // Write the continuation bytes in reverse order first
    for (int cont_index = size - 1; cont_index > 0; cont_index--)
    {
        utf8_t cont = codepoint & ~UTF8_CONTINUATION_MASK;
        cont |= UTF8_CONTINUATION_VALUE;

        utf8[index + cont_index] = cont;
        codepoint >>= UTF8_CONTINUATION_CODEPOINT_BITS;
    }

    // Write the leading byte
    utf8_pattern pattern = utf8_leading_bytes[size - 1];

    utf8_t lead = codepoint & ~(pattern.mask);
    lead |= pattern.value;

    utf8[index] = lead;

    return size;
}

size_t utf16_to_utf8(utf16_t const* utf16, size_t utf16_len, utf8_t* utf8, size_t utf8_len)
{
    // The next codepoint that will be written in the UTF-8 string
    // or the size of the required buffer if utf8 is NULL
    size_t utf8_index = 0;

    for (size_t utf16_index = 0; utf16_index < utf16_len; utf16_index++)
    {
        codepoint_t codepoint = decode_utf16(utf16, utf16_len, &utf16_index);

        if (utf8 == NULL)
            utf8_index += calculate_utf8_size(codepoint);
        else
            utf8_index += encode_utf8(codepoint, utf8, utf8_len, utf8_index);
    }

    return utf8_index;
}


size_t utf8_to_utf16(utf8_t const* utf8, size_t utf8_len, utf16_t* utf16, size_t utf16_len)
{
    fprintf(stderr, "NOT IMPLEMENTED YET");
    return 0;
}
