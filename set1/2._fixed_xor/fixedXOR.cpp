#include <iostream>
#include <string>


//=============================================
// A function that takes two equal-length buffers and produces their XOR combination
// To use provide 2 hex strings as arguments
// Exercise: https://cryptopals.com/sets/1/challenges/2
//=============================================


std::string hex2bin(std::string hexString);
std::string charhex2bin(char c);
std::string stringXOR(std::string binString1, std::string binString2);
std::string bin2hex(std::string binString);
char charbin2hex(std::string charBin);


int main(int argc, char* argv[])
{
    std::string hexString1       = argv[1];
    std::string hexString2       = argv[2];
    std::string binString1       = hex2bin(hexString1);
    std::string binString2       = hex2bin(hexString2);
    std::string xorBinString     = stringXOR(binString1, binString2);
    std::string xorHexString     = bin2hex(xorBinString);

    std::cout << "Hex string no. 1:      " << hexString1 << std::endl << std::endl;
    std::cout << "Hex string no. 2:      " << hexString2 << std::endl << std::endl;
    std::cout << "Bin string no. 1:      " << binString1 << std::endl << std::endl;
    std::cout << "Bin string no. 2:      " << binString2 << std::endl << std::endl;
    std::cout << "Bin XOR string:        " << xorBinString  << std::endl << std::endl;
    std::cout << "Hex XOR string:        " << xorHexString << std::endl << std::endl;

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

std::string bin2hex(std::string binString) {
    std::string hexString;
    for (int i = 0; i < binString.length(); i += 4) {
        hexString = hexString.append(std::string(1, charbin2hex(binString.substr(i, 4))));
    }
    return hexString;
}

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

std::string stringXOR(std::string binString1, std::string binString2) {
    std::string xorString = "";
    for (int i = 0; i < binString1.length(); i++) {
        if (binString1[i] == binString2[i]) {
            xorString += '0';
        }
        else {
            xorString += '1';
        }
    }
    return xorString;
}
