#ifndef CONVERTERS_H
#define CONVERTERS_H

#include <string>

// ==============================
// CONVERTERS - Format conversion utilities
//
// Functions to convert between:
//  - Base64, Hex, Binary (string representations)
//  - ASCII text
//
// Includes helper functions for mapping individual characters/bits.
//
// All functions take input as std::string_view and return std::string.
// ==============================

// Converts a Base64 encoded string to its Hexadecimal representation.
std::string base64ToHex(std::string_view base64Str);

// Converts a Base64 encoded string to its Binary representation as a string of '0' and '1'.
std::string base64ToBin(std::string_view base64Str);

// Converts a Hexadecimal string to a Base64 encoded string.
std::string hex2base64(std::string_view hexString);

// Converts a Binary string (bits as '0' and '1') to a Base64 encoded string.
std::string bin2base64(std::string_view binString);

// Converts a Binary string to a Hexadecimal string.
std::string bin2hex(std::string_view binString);

// Converts a Hexadecimal string to a Binary string.
std::string hex2bin(std::string_view hexString);

// Converts a Hexadecimal string representing ASCII bytes into an ASCII string.
std::string hex2ascii(std::string_view hexStr);

// Converts an ASCII string into its Hexadecimal representation.
std::string ascii2hex(std::string_view asciiStr);

// Converts a Binary string (multiple of 8 bits) into an ASCII string.
std::string bin2ascii(std::string_view binString);

// Converts an ASCII string into a Binary string (bits as '0' and '1').
std::string ascii2bin(std::string_view asciiStr);

// Converts a Base64 encoded string directly into an ASCII string.
std::string base64Toascii(std::string_view base64Str);

// Converts a 6-bit binary string to the corresponding Base64 character.
std::string charbin2base64(std::string_view bin);

// Converts a 4-bit binary string to the corresponding Hex character.
char charbin2hex(std::string_view charBin);

// Converts a Hex character to the corresponding 4-bit binary string.
std::string charhex2bin(char c);

#endif // CONVERTERS_H