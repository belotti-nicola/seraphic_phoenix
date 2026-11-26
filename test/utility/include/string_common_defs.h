#pragma once

typedef enum gen_kind_t
{
    GEN_SAME,       // "aaaa..."
    GEN_ALPHA,      // "abc...zabc..."
    GEN_DIGITS,     // "0123456789012..."
    GEN_COUNTER,    // "000:001:002:..."
    GEN_BYTES,      // byte 1..255                256 => 0x80 => '\0'
    GEN_RANDOM,     // ASCII (seed reproducible)
    GEN_CASE_MIX,   // "aBcDeF..."                like GEN_ALPHA but case-sensitive
    GEN_SPACES,     // " \t\n\r"

} gen_kind_t;