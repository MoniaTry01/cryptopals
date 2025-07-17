/*
    This code performs a letter frequency analysis on ASCII strings encoded with single-byte XOR cipher
    The encoded text should be provided as a hex string
    The code brute-forces all key combinations and performs Chi-Square test to deduce the most probable original string
    This code only works correctly for text in english language
    If result is incorrect, please check if correct string is among candidate strings
*/

#include <iostream>
#include <map>
#include <cmath>
#include "cryptopalstools.h"


struct XORResult {
    int key;
    double chi2;
    std::string decodedText;
};


XORResult iterateXORKeys(std::string inputStr);
double singleCharFittingQuotient(int inputStrLetterOccurance, int strLen, char letter);
double singleKeyFittingQuotient(std::string inputStr);
int countCharOccurance(std::string inputStr, char c);
double charFreqTable(char c);


int main(int argc, char* argv[])
{
    std::string inputStr = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736"; // "Cooking MC's like a pound of bacon"
    //std::string inputStr = "162A2B31622B3162236236273136623136302B2C25"; // "This is a test string"
    //std::string inputStr = "0c302175343b3a213d3027753d302d752621273c3b32"; // "Yet another hex string"
    std::string asciiStr = hex2ASCII(inputStr);
    XORResult decoded = iterateXORKeys(asciiStr);
    std::cout << "Best key: " << decoded.key << " (0x" << std::hex << decoded.key << ")" << std::endl;
    std::cout << "Chi^2: " << decoded.chi2 << std::endl;
    std::cout << "Decoded text: " << decoded.decodedText << std::endl;
    return 0;
}

XORResult iterateXORKeys(std::string inputStr) {
    std::cout << "Candidate strings: " << std::endl << std::endl;
    std::string finalStr;
    double lowestFitQuotResult = 1e9;
    int bestKey = -1;
    for (int i = 0; i < 256; i++) {
        std::string tempStr;
        int letterOrSpaceCount = 0;
        for (char c : inputStr) {
            char decoded = c ^ i;
            if (std::isalpha(static_cast<unsigned char>(decoded)) || decoded == ' ')
                letterOrSpaceCount++;
            tempStr += decoded;
        }

        // Uncomment to list all decoded strings
        //std::cout << "Key: " << i << " (0x" << std::hex << i << ")" << std::endl;
        //std::cout << "Decoded string: " << tempStr << std::endl << std::endl;
           
        // Only continue if at least 60% of string is letters or spaces
        if (static_cast<double>(letterOrSpaceCount) / inputStr.length() < 0.6)
            continue;

        double fitQuotResult = singleKeyFittingQuotient(tempStr);
        
        // List all decoded strings with sufficiently low Chi^2 
        if (fitQuotResult < 50.0) {
            std::cout << "Key: " << i << std::endl;
            std::cout << "Chi^2: " << fitQuotResult << std::endl;
            std::cout << "Decoded string: " << tempStr << std::endl << std::endl;
        }

        if (fitQuotResult < lowestFitQuotResult) {
            lowestFitQuotResult = fitQuotResult;
            finalStr = tempStr;
            bestKey = i;
        }
    }
    XORResult result;
    result.key = bestKey;
    result.chi2 = lowestFitQuotResult;
    result.decodedText = finalStr;
    return result;
}

double singleKeyFittingQuotient(std::string inputStr) {
    const std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    int strLen = inputStr.length();
    double singleStepFittingQuotient = 0;
    for (int i = 0; i < letters.length(); i++) {
        int realOcc = countCharOccurance(inputStr, letters[i]);
        singleStepFittingQuotient += singleCharFittingQuotient(realOcc, strLen, letters[i]);
    }
    return singleStepFittingQuotient;
}

double singleCharFittingQuotient(int inputStrLetterOccurance, int strLen, char letter) {
    double expected = (charFreqTable(letter) / 100.0) * strLen;
    double occured = static_cast<double>(inputStrLetterOccurance);
    if (expected < 1e-6) return 0;
    return std::pow((occured - expected), 2) / expected;
}

int countCharOccurance(std::string inputStr, char c) {
    int counter = 0;
    for (char ch : inputStr) {
        if (std::toupper(static_cast<unsigned char>(ch)) == c)
            counter++;
    }
    return counter;
}

double charFreqTable(char c) {
    // Letter frequencies based on this: https://pi.math.cornell.edu/~mec/2003-2004/cryptography/subs/frequencies.html
    std::map<char, double> letterFreq = {
        {'A', 8.12}, {'B', 1.49}, {'C', 2.71}, {'D', 4.32}, {'E', 12.02}, {'F', 2.30}, {'G', 2.03}, {'H', 5.92}, {'I', 7.31},
        {'J', 0.10}, {'K', 0.69}, {'L', 3.98}, {'M', 2.61}, {'N', 6.95}, {'O', 7.68}, {'P', 1.82}, {'Q', 0.11}, {'R', 6.02},
        {'S', 6.28}, {'T', 9.10}, {'U', 2.88}, {'V', 1.11}, {'W', 2.09}, {'X', 0.17}, {'Y', 2.11}, {'Z', 0.07}, {' ', 13.00}
    };
    return letterFreq.at(c);
}
