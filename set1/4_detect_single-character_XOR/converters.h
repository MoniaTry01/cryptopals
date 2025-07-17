#ifndef CONVERTERS_H
#define CONVERTERS_H

#include <string>

std::string base64ToHex(const std::string& base64Str);
std::string base64ToBin(const std::string& base64Str);
std::string hex2base64(const std::string& hexString);
std::string bin2base64(const std::string& binString);
std::string bin2hex(const std::string& binString);
std::string hex2bin(const std::string& hexString);

std::string hex2ASCII(const std::string& hexStr);
std::string ASCII2hex(const std::string& asciiStr);

std::string charbin2base64(const std::string& bin);
char charbin2hex(const std::string& charBin);
std::string charhex2bin(char c);

#endif // CONVERTERS_H

