#include <iostream>
#include <string>
#include <cctype>
#include <bitset>

std::string base64ToHex(std::string base64Str); // base64 to hex converter
std::string base64ToBin(std::string base64Str); // base64 to bin converter
std::string hex2base64(std::string hexString);  // hex to base64 converter
std::string bin2base64(std::string binString);  // bin to base64 converter
std::string charbin2base64(std::string bin);    // binary to base64 table
std::string bin2hex(std::string binString);     // binary to hex converter
char charbin2hex(std::string charBin);          // binary to hex table
std::string hex2bin(std::string hexString);     // hex to binary converter
std::string charhex2bin(char c);                // hex to binary table
std::string hex2ASCII(std::string hexStr);      // hex to ASCII converter
std::string fixedXOR(std::string hexString1, std::string hexString2);       // Fixed XOR - A function that takes two equal-length hex strings and produces their XOR combination
std::string binStringXOR(std::string binString1, std::string binString2);   // Fixed XOR - A function that takes two equal-length binary strings and produces their XOR combination


//=============================================
// base64 to hex converter
//=============================================

std::string base64ToHex(std::string base64Str)
{
    std::string binString = base64ToBin(base64Str);
    return bin2hex(binString);
}

//=============================================
// base64 to bin converter
//=============================================

std::string base64ToBin(std::string base64Str) {
    std::string binString;
    for (int i = 0; i < base64Str.length(); i++) {
        std::string currChar = std::bitset<6>(base64Str[i]).to_string();
        binString += currChar;
    }
    return binString;
}

//=============================================
// hex to base64 converter
//=============================================

std::string hex2base64(std::string hexString)
{
    std::string binString = hex2bin(hexString);
    std::string base64String = bin2base64(binString);
    return base64String;
}

//=============================================
// bin to base64 converter
//=============================================

std::string bin2base64(std::string binString) {
    std::string base64String;
    if (binString.length() % 6 > 0)
        binString = binString.append(6 - binString.length() % 6, '0');
    for (int i = 0; i < binString.length(); i += 6) {
        base64String = base64String.append(charbin2base64(binString.substr(i, 6)));
    }
    if (base64String.length() % 4 > 0)
        base64String += "=";
    return base64String;
}

//=============================================
// binary to base64 table
//=============================================

std::string charbin2base64(std::string bin) {
    static const std::string base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int base64index = std::bitset<6>(bin).to_ulong();
    return std::string(1, base64[base64index]);
}

//=============================================
// binary to hex converter
//=============================================

std::string bin2hex(std::string binString) {
    std::string hexString;
    for (int i = 0; i < binString.length(); i += 4) {
        hexString = hexString.append(std::string(1, charbin2hex(binString.substr(i, 4))));
    }
    return hexString;
}

//=============================================
// binary to hex table
//=============================================

char charbin2hex(std::string charBin) {
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
    return '\0';
}

//=============================================
// hex to binary converter
//=============================================

std::string hex2bin(std::string hexString) {
    std::string binString;
    for (int i = 0; i < hexString.length(); i++)
        binString = binString.append(charhex2bin(hexString[i]));
    return binString;
}

//=============================================
// hex to binary table
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
// hex to ASCII converter
//=============================================
std::string hex2ASCII(std::string hexStr) {
    std::string asciiStr;
    for (int i = 0; i < hexStr.length(); i += 2) {
        std::string charHex = hexStr.substr(i, 2);
        char charASCII = (char)std::stoi(charHex, nullptr, 16);
        asciiStr.push_back(charASCII);
    }
    return asciiStr;
}

//=============================================
// XOR section
// ============================================

//============================================
// Fixed XOR
// A function that takes two equal-length hex strings and produces their XOR combination
//=============================================

std::string fixedXOR(std::string hexString1, std::string hexString2)
{
    std::string binString1 = hex2bin(hexString1);
    std::string binString2 = hex2bin(hexString2);
    std::string xorBinString = binStringXOR(binString1, binString2);
    std::string xorHexString = bin2hex(xorBinString);
    return xorHexString;
}

//============================================
// A function that takes two equal-length binary strings and produces their XOR combination
//=============================================

std::string binStringXOR(std::string binString1, std::string binString2) {
    std::string xorString = "";

    int len = 0;
    if (binString1.length() < binString2.length()) len = binString1.length();
    else len = binString2.length();

    for (int i = 0; i < len; i++) {
        if (binString1[i] == binString2[i]) {
            xorString += '0';
        }
        else {
            xorString += '1';
        }
    }
    return xorString;
}