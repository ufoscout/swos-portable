/*
 * ConvertUTF.h stub for SimpleIni SI_CONVERT_GENERIC mode.
 * The game uses CSimpleIniA (narrow chars only), so the wchar_t converter
 * (SI_ConvertW) is never instantiated and these functions are never called.
 * This stub provides only the type declarations needed to compile SimpleIni.h.
 *
 * Based on the Unicode Inc. reference implementation (public domain).
 */
#pragma once

typedef unsigned char    UTF8;
typedef unsigned short   UTF16;
typedef unsigned int     UTF32;

typedef enum {
    conversionOK,
    sourceExhausted,
    targetExhausted,
    sourceIllegal
} ConversionResult;

typedef enum {
    strictConversion = 0,
    lenientConversion
} ConversionFlags;

#ifdef __cplusplus
extern "C" {
#endif

ConversionResult ConvertUTF8toUTF16(const UTF8** sourceStart, const UTF8* sourceEnd,
    UTF16** targetStart, UTF16* targetEnd, ConversionFlags flags);
ConversionResult ConvertUTF16toUTF8(const UTF16** sourceStart, const UTF16* sourceEnd,
    UTF8** targetStart, UTF8* targetEnd, ConversionFlags flags);
ConversionResult ConvertUTF8toUTF32(const UTF8** sourceStart, const UTF8* sourceEnd,
    UTF32** targetStart, UTF32* targetEnd, ConversionFlags flags);
ConversionResult ConvertUTF32toUTF8(const UTF32** sourceStart, const UTF32* sourceEnd,
    UTF8** targetStart, UTF8* targetEnd, ConversionFlags flags);

#ifdef __cplusplus
}
#endif
