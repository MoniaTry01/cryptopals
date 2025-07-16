#include <iostream>
#include <string>
#include <cctype>
#include <bitset>

//=============================================
// A simple hex to base64 converter
// To use provide hex string as an argument
// Exercise: https://cryptopals.com/sets/1
//=============================================

std::string hex2bin(std::string hexString);
std::string charhex2bin(char c);
std::string bin2base64(std::string binString);
std::string charbin2base64(std::string bin);


int main(int argc, char* argv[])
{
    std::string hexString = argv[1];
    std::cout << "Hex string:      " << hexString << std::endl << std::endl;
    std::string binString = hex2bin(hexString);
    std::cout << "Bin string:      " << binString << std::endl << std::endl;
    std::string base64String = bin2base64(binString);
    std::cout << "Base64 string:   " << base64String << std::endl << std::endl;
    return 0;
}


std::string hex2bin(std::string hexString) {
    std::string binString;
    for (int i = 0; i < hexString.length(); i++)
        binString = binString.append(charhex2bin(hexString[i]));
    return binString;
}

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

std::string charbin2base64(std::string bin) {
    static const std::string base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int base64index = std::bitset<6>(bin).to_ulong();
    return std::string(1, base64[base64index]);
}
