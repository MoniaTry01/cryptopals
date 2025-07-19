#include "converters.h"
#include <bitset>
#include <algorithm>
#include <cctype>
#include <stdexcept>

//=============================================
// Base64 to hex converter
// Takes:
//      base64Str - input Base64 encoded string
// Returns:
//      Hexadecimal representation of decoded Base64 data
//=============================================

std::string base64ToHex(std::string_view base64Str)
{
    std::string binString = base64ToBin(base64Str);
    return bin2hex(binString);
}

//=============================================
// Base64 character to decimal value lookup
// Takes:
//      c - single Base64 character
// Returns:
//      Decimal value (0-63) corresponding to Base64 char, or -1 if invalid
//=============================================

int base64CharToValue(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1; // incorrect char
}

//=============================================
// Base64 to binary converter
// Takes:
//      base64Str - Base64 encoded string
// Returns:
//      Binary string representation of decoded Base64 data (6 bits per char)
//=============================================

std::string base64ToBin(std::string_view base64Str) {
    size_t len = base64Str.length();
    std::string binString(len * 6, '0');
    for (size_t i = 0; i < len; i++) {
        int val = base64CharToValue(base64Str[i]);
        if (val < 0) continue; // incorrect char
        std::string currBits = std::bitset<6>(val).to_string();
        std::copy(currBits.begin(), currBits.end(), binString.begin() + i * 6);
    }
    return binString;
}

//=============================================
// Hex to Base64 converter
// Takes:
//      hexString - input hexadecimal string
// Returns:
//      Base64 encoded string representing input hex data
//=============================================

std::string hex2base64(std::string_view hexString)
{
    std::string binString = hex2bin(hexString);
    std::string base64String = bin2base64(binString);
    return base64String;
}

//=============================================
// Binary to Base64 converter
// Takes:
//      binString - binary string input
// Returns:
//      Base64 encoded string representing input binary data (with padding)
//=============================================

std::string bin2base64(std::string_view binString) {
    std::string temp = std::string{ binString };
    std::string base64String;
    if (temp.length() % 6 > 0)
        temp.append(6 - temp.length() % 6, '0');
    for (size_t i = 0; i < temp.length(); i += 6) {
        base64String.append(charbin2base64(temp.substr(i, 6)));
    }
    if (base64String.length() % 4 > 0)
        base64String.append(4 - base64String.length() % 4, '=');
    return base64String;
}


//=============================================
// Binary (6-bit) to Base64 character lookup
// Takes:
//      bin - string of 6 bits ("0" or "1")
// Returns:
//      Corresponding Base64 character as string
//=============================================

std::string charbin2base64(std::string_view bin) {
    static const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (bin.size() != 6) return "?";
    int index = static_cast<int>(std::bitset<6>(std::string{ bin }).to_ulong());
    return std::string(1, base64[index]);
}


//=============================================
// Binary to hexadecimal converter
// Takes:
//      binString - binary string input (length multiple of 4)
// Returns:
//      Hexadecimal string representation of binary input
//=============================================

std::string bin2hex(std::string_view binString) {
    std::string hexString;
    for (int i = 0; i < binString.length(); i += 4) {
        hexString.append(std::string(1, charbin2hex(binString.substr(i, 4))));
    }
    return hexString;
}

//=============================================
// Binary (4-bit) to hexadecimal character lookup
// Takes:
//      charBin - string of 4 bits ("0" or "1")
// Returns:
//      Single hexadecimal character corresponding to input bits
//=============================================

char charbin2hex(std::string_view charBin) {
    static const char lookup[] = "0123456789ABCDEF";
    if (charBin.size() != 4) return '?';
    int val = static_cast<int>(std::bitset<4>(std::string{ charBin }).to_ulong());
    return lookup[val];
}


//=============================================
// Hexadecimal to binary converter
// Takes:
//      hexString - hexadecimal string input
// Returns:
//      Binary string representation of hex input (4 bits per hex digit)
//=============================================

std::string hex2bin(std::string_view hexString) {
    size_t len = hexString.length();
    std::string binString(len * 4, '0');
    for (size_t i = 0; i < len; i++) {
        std::string bits = charhex2bin(hexString[i]);
        std::copy(bits.begin(), bits.end(), binString.begin() + i * 4);
    }
    return binString;
}

//=============================================
// Hexadecimal character to binary lookup
// Takes:
//      c - single hexadecimal character (0-9, A-F)
// Returns:
//      4-bit binary string representing hex digit
//=============================================

std::string charhex2bin(char c) {
    switch (toupper(c)) {
    case '0': return "0000";
    case '1': return "0001";
    case '2': return "0010";
    case '3': return "0011";
    case '4': return "0100";
    case '5': return "0101";
    case '6': return "0110";
    case '7': return "0111";
    case '8': return "1000";
    case '9': return "1001";
    case 'A': return "1010";
    case 'B': return "1011";
    case 'C': return "1100";
    case 'D': return "1101";
    case 'E': return "1110";
    case 'F': return "1111";
    default:  return "";
    }
}

//=============================================
// Hexadecimal to ASCII converter
// Takes:
//      hexStr - hexadecimal string representing ASCII encoded data
// Returns:
//      ASCII string decoded from hex input
//=============================================

std::string hex2ascii(std::string_view hexStr) {
    auto hexCharToInt = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        throw std::invalid_argument("Invalid hex character");
        };

    size_t len = hexStr.length();
    std::string asciiStr(len / 2, '\0');
    for (size_t i = 0; i < len; i += 2) {
        int high = hexCharToInt(hexStr[i]);
        int low = hexCharToInt(hexStr[i + 1]);
        asciiStr[i / 2] = static_cast<char>((high << 4) | low);
    }
    return asciiStr;
}

//=============================================
// ASCII to hexadecimal converter
// Takes:
//      asciiStr - ASCII string input
// Returns:
//      Hexadecimal string representation of ASCII data
//=============================================

std::string ascii2hex(std::string_view asciiStr) {
    const char* hexChars = "0123456789abcdef";
    size_t len = asciiStr.length();
    std::string hexStr(len * 2, '\0');
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = asciiStr[i];
        hexStr[2 * i] = hexChars[(c >> 4) & 0x0F];;  // bit shift, first hex number, 0x0F zeros unnecessary bits, just in case
        hexStr[2 * i + 1] = hexChars[c & 0x0F];      // second hex number
    }
    return hexStr;
}


//=============================================
// Binary to ASCII converter
// Takes:
//      binString - binary string input (length must be a multiple of 8)
// Returns:
//      ASCII string corresponding to the binary input
// Throws:
//      std::invalid_argument if binString length is not a multiple of 8
//=============================================

std::string bin2ascii(std::string_view binString) {
    if (binString.size() % 8 != 0) {
        throw std::invalid_argument("String not divisible by 8");
    }
    std::string result;
    result.reserve(binString.size() / 8);
    for (size_t i = 0; i < binString.size(); i += 8) {
        char c = static_cast<char>(std::bitset<8>(std::string{ binString.substr(i, 8) }).to_ulong());
        result.push_back(c);
    }
    return result;
}


//=============================================
// ASCII to binary converter
// Takes:
//      asciiStr - ASCII string input
// Returns:
//      Concatenated binary string representation of the entire ASCII string
//=============================================

std::string ascii2bin(std::string_view asciiStr) {
    std::string result;
    result.reserve(asciiStr.size() * 8);
    for (char c : asciiStr) {
        result += std::bitset<8>(c).to_string();
    }
    return result;
}


//=============================================
// Base64 to ASCII converter
// Takes:
//      base64Str - Base64 encoded string
// Returns:
//      ASCII string decoded from Base64 input
//=============================================

std::string base64Toascii(std::string_view base64Str) {
    std::string binString = base64ToBin(base64Str);
    size_t totalBits = (base64Str.length() * 6);
    size_t paddingChars = 0;
    if (!base64Str.empty()) {
        if (base64Str.back() == '=') paddingChars++;
        if (base64Str.length() > 1 && base64Str[base64Str.length() - 2] == '=') paddingChars++;
    }
    totalBits -= paddingChars * 6;
    totalBits = (totalBits / 8) * 8;
    binString.resize(totalBits);

    return bin2ascii(binString);
}