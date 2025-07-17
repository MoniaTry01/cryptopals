#include "xor_utils.h"
#include "converters.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <vector>

//=============================================
// Fixed XOR of two equal-length hex strings
// Takes:
//      hexString1, hexString2 - hex strings of equal length
// Returns:
//      Hex string representing XOR combination of inputs
//=============================================

std::string fixedXOR(const std::string& hexString1, const std::string& hexString2)
{
    std::string binString1 = hex2bin(hexString1);
    std::string binString2 = hex2bin(hexString2);
    std::string xorBinString = binStringXOR(binString1, binString2);
    std::string xorHexString = bin2hex(xorBinString);
    return xorHexString;
}

//=============================================
// Fixed XOR of two equal-length binary strings
// Takes:
//      binString1, binString2 - binary strings of equal length
// Returns:
//      Binary string representing XOR combination of inputs
//=============================================

std::string binStringXOR(const std::string& binString1, const std::string& binString2) {

    size_t len = std::min(binString1.length(), binString2.length());
    std::string xorString(len, '0');

    for (int i = 0; i < len; i++) {
        if (binString1[i] != binString2[i]) {
            xorString[i] = '1';
        }
    }
    return xorString;
}

//=============================================
// Single-byte XOR cipher frequency analysis
// Takes:
//      encodedStr - hex string encoded with single-byte XOR cipher
//      chi2threshold - Chi-square threshold for candidate acceptance
//      printableCharThreshold - minimum ratio of printable chars required
//      additionalInfo - if true, returns extra diagnostic strings
// Returns:
//      Vector of candidate decoded ASCII strings passing frequency tests
// Note:
//     This function performs a letter frequency analysis on ASCII strings encoded with single-byte XOR cipher
//     The code brute - forces all key combinations and performs Chi - Square test to deduce the most probable original string
//     This code only works correctly for text in english language
//     If result is incorrect, please check if correct string is among candidate strings
//=============================================

std::vector<std::string> singleByteXORFreqAnalysis(const std::string& encodedStr, int chi2threshold, double printableCharTreshhold, bool additionalInfo)
{
    std::string asciiStr = hex2ASCII(encodedStr);
    std::vector<std::string> decodedStrings = iterateXORKeys(asciiStr, chi2threshold, printableCharTreshhold, additionalInfo);
    return decodedStrings;
}


//=============================================
// Iterate through all possible single-byte XOR keys
// Performs frequency analysis using Chi-square tests to find candidates
// Takes:
//      inputStr - ASCII string to decode (XOR encrypted)
//      chi2threshold - Chi-square threshold for candidate acceptance
//      printableCharThreshold - minimum ratio of printable chars required
//      additionalInfo - if true, adds diagnostic info strings to results
// Returns:
//      Vector of candidate decoded strings passing frequency analysis
//=============================================

std::vector<std::string> iterateXORKeys(const std::string& inputStr, int chi2threshold, double printableCharTreshhold, bool additionalInfo)
{
    std::vector<std::string> candidateStrings;
    for (int i = 0; i < 256; i++) {
        std::string tempStr(inputStr.length(), '\0');
        int letterOrSpaceCount = 0;
        for (size_t j = 0; j < inputStr.length(); j++) {
            char decoded = inputStr[j] ^ i;
            tempStr[j] = decoded;
            if (std::isalpha(static_cast<unsigned char>(decoded)) || decoded == ' ')
                letterOrSpaceCount++;
        }

        // Only continue if certain anount of char in string is letters or spaces
        if (static_cast<double>(letterOrSpaceCount) / inputStr.length() < printableCharTreshhold)
            continue;

        double fitQuotResult = singleKeyFittingQuotient(tempStr);

        // List all decoded strings with sufficiently low Chi^2 
        if (fitQuotResult < chi2threshold) {
            if (additionalInfo) {
                std::string info = "\nOriginal string (ASCII): " + inputStr;
                candidateStrings.push_back(info);
                info = "Original string (HEX): " + ASCII2hex(inputStr);
                candidateStrings.push_back(info);
                info = "Key (dec): " + std::to_string(i);
                candidateStrings.push_back(info);
                info = "Chi^2: " + std::to_string(fitQuotResult);
                candidateStrings.push_back(info);
            }
            candidateStrings.push_back(tempStr);
        }
    }
    return candidateStrings;
}


//=============================================
// Compute Chi-square fitting quotient for string (for single key)
// Measures how closely letter frequency in inputStr matches English
// Takes:
//      inputStr - ASCII string
// Returns:
//      Double value representing Chi-square fitting score (lower is better)
//=============================================

double singleKeyFittingQuotient(const std::string& inputStr) {
    const std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    size_t strLen = inputStr.length();
    double singleStepFittingQuotient = 0;
    for (size_t i = 0; i < letters.length(); i++) {
        int realOcc = countCharOccurance(inputStr, letters[i]);
        singleStepFittingQuotient += singleCharFittingQuotient(realOcc, static_cast<int>(strLen), letters[i]);
    }
    return singleStepFittingQuotient;
}


//=============================================
// Compute Chi-square fitting quotient for a single character
// Takes:
//      inputStrLetterOccurance - count of letter occurrences in string
//      strLen - total length of string
//      letter - character to analyze
// Returns:
//      Chi-square contribution of the character
//=============================================

double singleCharFittingQuotient(int inputStrLetterOccurance, int strLen, char letter) {
    double expected = (charFreqTable(letter) / 100.0) * strLen;
    double occured = static_cast<double>(inputStrLetterOccurance);
    if (expected < 1e-6) return 0;
    return ((occured - expected) * (occured - expected)) / expected;
}


//=============================================
// Count occurrences of character (case-insensitive) in string
// Takes:
//      inputStr - ASCII string
//      c - character to count
// Returns:
//      Number of occurrences of character c in inputStr
//=============================================

int countCharOccurance(const std::string& inputStr, char c) {
    int counter = 0;
    for (char ch : inputStr) {
        if (std::toupper(static_cast<unsigned char>(ch)) == c)
            counter++;
    }
    return counter;
}

//=============================================
// Letter frequency lookup table for English letters and space
// Takes:
//      c - character (A-Z or space)
// Returns:
//      Expected frequency percentage in English text
//=============================================

double charFreqTable(char c) {
    // Letter frequencies based on this: https://pi.math.cornell.edu/~mec/2003-2004/cryptography/subs/frequencies.html
    static const double freqTable[27] = {
        8.12, 1.49, 2.71, 4.32, 12.02, 2.30, 2.03, 5.92,
        7.31, 0.10, 0.69, 3.98, 2.61, 6.95, 7.68, 1.82,
        0.11, 6.02, 6.28, 9.10, 2.88, 1.11, 2.09, 0.17,
        2.11, 0.07, 13.00  // last char is space, rest is letters "ABC... XYZ[space]"
    };
    c = std::toupper(static_cast<unsigned char>(c));
    if (c == ' ') return freqTable[26]; // if space
    if (c >= 'A' && c <= 'Z') return freqTable[c - 'A']; // rest of alphabet
    return 0.0;
}
