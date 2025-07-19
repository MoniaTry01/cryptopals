#ifndef CONVERTERS_H
#define CONVERTERS_H

#include <string>

std::string base64ToHex(std::string_view base64Str);
std::string base64ToBin(std::string_view base64Str);
std::string hex2base64(std::string_view hexString);
std::string bin2base64(std::string_view binString);
std::string bin2hex(std::string_view binString);
std::string hex2bin(std::string_view hexString);

std::string hex2ascii(std::string_view hexStr);
std::string ascii2hex(std::string_view asciiStr);
std::string bin2ascii(std::string_view binString);
std::string ascii2bin(std::string_view asciiStr);
std::string base64Toascii(std::string_view base64Str);

std::string charbin2base64(std::string_view bin);
char charbin2hex(std::string_view charBin);
std::string charhex2bin(char c);

#endif // CONVERTERS_H

