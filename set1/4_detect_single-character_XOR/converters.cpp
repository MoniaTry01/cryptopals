#include "converters.h"
#include <bitset>
#include <algorithm>
#include <cctype>

//=============================================
// Base64 to hex converter
// Takes:
//      base64Str - input Base64 encoded string
// Returns:
//      Hexadecimal representation of decoded Base64 data
//=============================================

std::string base64ToHex(const std::string& base64Str)
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

std::string base64ToBin(const std::string& base64Str) {
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

std::string hex2base64(const std::string& hexString)
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

std::string bin2base64(const std::string& binString) {
    std::string temp = binString;
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

std::string charbin2base64(const std::string& bin) {
    static const std::string base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int base64index = std::bitset<6>(bin).to_ulong();
    return std::string(1, base64[base64index]);
}

//=============================================
// Binary to hexadecimal converter
// Takes:
//      binString - binary string input (length multiple of 4)
// Returns:
//      Hexadecimal string representation of binary input
//=============================================

std::string bin2hex(const std::string& binString) {
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

char charbin2hex(const std::string& charBin) {
    if (charBin == "0000") return '0';
    if (charBin == "0001") return '1';
    if (charBin == "0010") return '2';
    if (charBin == "0011") return '3';
    if (charBin == "0100") return '4';
    if (charBin == "0101") return '5';
    if (charBin == "0110") return '6';
    if (charBin == "0111") return '7';
    if (charBin == "1000") return '8';
    if (charBin == "1001") return '9';
    if (charBin == "1010") return 'A';
    if (charBin == "1011") return 'B';
    if (charBin == "1100") return 'C';
    if (charBin == "1101") return 'D';
    if (charBin == "1110") return 'E';
    if (charBin == "1111") return 'F';
    return '?';
}

//=============================================
// Hexadecimal to binary converter
// Takes:
//      hexString - hexadecimal string input
// Returns:
//      Binary string representation of hex input (4 bits per hex digit)
//=============================================

std::string hex2bin(const std::string& hexString) {
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

std::string hex2ASCII(const std::string& hexStr) {
    size_t len = hexStr.length();
    std::string asciiStr(hexStr.length() / 2, '\0');
    for (size_t i = 0; i < len; i += 2) {
        std::string charHex = hexStr.substr(i, 2);
        asciiStr[i / 2] = static_cast<char>(std::stoi(charHex, nullptr, 16));
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

std::string ASCII2hex(const std::string& asciiStr) {
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