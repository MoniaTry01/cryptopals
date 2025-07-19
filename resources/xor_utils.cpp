#include "xor_utils.h"
#include "converters.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

//=============================================
// Fixed XOR of two equal-length hex strings
// Takes:
//      hexString1, hexString2 - hex strings of equal length
// Returns:
//      Hex string representing XOR combination of inputs
//=============================================

std::string XOR_xorEqualHexString(std::string_view hexString1, std::string_view hexString2)
{
    std::string binString1 = hex2bin(hexString1);
    std::string binString2 = hex2bin(hexString2);
    std::string xorBinString = XOR_xorEqualBinString(binString1, binString2);
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

std::string XOR_xorEqualBinString(std::string_view binString1, std::string_view binString2) {

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

std::vector<std::string> XOR_singleByteFreqAnalysis(std::string_view encodedStr, int chi2threshold, double printableCharTreshhold, bool additionalInfo, bool onlyBestFit)
{
    std::string asciiStr = hex2ascii(encodedStr);
    std::vector<std::string> decodedStrings = XOR_iterateKeys_str(asciiStr, chi2threshold, printableCharTreshhold, additionalInfo, onlyBestFit);
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
//      onlyBestFit - if true, return only the result with lowest Chi^2 (vector with single string)
// Returns:
//      Vector of candidate decoded strings passing frequency analysis
//=============================================

std::vector<std::string> XOR_iterateKeys_str(std::string_view inputStr, int chi2threshold, double printableCharTreshhold, bool additionalInfo, bool onlyBestFit)
{
    double bestFit = std::numeric_limits<double>::max();
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

        if (onlyBestFit) {
            if (fitQuotResult < bestFit) {
                bestFit = fitQuotResult;
                candidateStrings.clear();
                candidateStrings.push_back(tempStr);
            }
        } else {
            if (fitQuotResult < chi2threshold) {
                if (additionalInfo) {
                    std::string info = "\nOriginal string (ASCII): " + std::string{ inputStr };
                    candidateStrings.push_back(info);
                    info = "Original string (HEX): " + ascii2hex(inputStr);
                    candidateStrings.push_back(info);
                    info = "Key (dec): " + std::to_string(i);
                    candidateStrings.push_back(info);
                    info = "Chi^2: " + std::to_string(fitQuotResult);
                    candidateStrings.push_back(info);
                }
                candidateStrings.push_back(tempStr);
            }
        }
    }
    return candidateStrings;
}


// Same as above but returns only keys (or only best key if onlyBestFit == true)

std::vector<int> XOR_iterateKeys_keys(std::string_view inputStr, int chi2threshold, double printableCharTreshhold, bool onlyBestFit)
{
    double bestFit = std::numeric_limits<double>::max();
    std::vector<int> candidateKeys;
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

        if (onlyBestFit) {
            if (fitQuotResult < bestFit) {
                bestFit = fitQuotResult;
                candidateKeys.clear();
                candidateKeys.push_back(i);
            }
        }
        else {
            if (fitQuotResult < chi2threshold) {
                candidateKeys.push_back(i);
            }
        }
    }
    return candidateKeys;
}


// Same as above but returns only ch^2 metric (or only best ch^2 if onlyBestFit == true)

std::vector<double> XOR_iterateKeys_chi2(std::string_view inputStr, int chi2threshold, double printableCharTreshhold, bool onlyBestFit)
{
    double bestFit = std::numeric_limits<double>::max();
    std::vector<double> bestFits;
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

        if (onlyBestFit) {
            if (fitQuotResult < bestFit) {
                bestFit = fitQuotResult;
                bestFits.clear();
                bestFits.push_back(i);
            }
        }
        else {
            if (fitQuotResult < chi2threshold) {
                bestFits.push_back(i);
            }
        }
    }
    return bestFits;
}


//=============================================
// Compute Chi-square fitting quotient for string (for single key)
// Measures how closely letter frequency in inputStr matches English
// Takes:
//      inputStr - ASCII string
// Returns:
//      Double value representing Chi-square fitting score (lower is better)
//=============================================

double singleKeyFittingQuotient(std::string_view inputStr) {
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

int countCharOccurance(std::string_view inputStr, char c) {
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


//=============================================
// XOR encryption with repeating key
// Takes:
//      inputStr - plaintext string to encrypt
//      key      - key string used for XOR encryption (repeated if shorter)
// Returns:
//      XOR-encrypted string
// Note:
//      Each character of the input is XOR-ed with the corresponding
//      character from the key (repeated as needed)
//=============================================

std::string XOR_repeatingKeyEncrypt(std::string_view inputStr, std::string_view key) {
    std::string encryptedStr = std::string{ inputStr };
    size_t keyLen = key.length();
    size_t inputLen = inputStr.length();
    for (size_t i = 0; i < inputLen; i++) {
        encryptedStr[i] = encryptedStr[i] ^ key[i % keyLen];
    }
    return encryptedStr;
}