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
double singleLetterFittingQuotient(int inputStrLetterOccurance, int strLen, char letter);
double singleStepAnalisys(std::string inputStr);
int calculateCharOccurance(std::string inputStr, char c);
double letterFreqTable(char c);


int main(int argc, char* argv[])
{
    //std::string inputStr = argv[1];
    std::string inputStr = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    std::string asciiStr = hex2ASCII(inputStr);
    //for (int i = 0; i < asciiStr.length(); i++)
    //    asciiStr[i] = std::toupper(asciiStr[i]);
    XORResult decoded = iterateXORKeys(asciiStr);
    std::cout << "Best key: " << decoded.key << " (0x" << std::hex << decoded.key << ")" << std::endl;
    std::cout << "Chi^2: " << decoded.chi2 << std::endl;
    std::cout << "Decoded text: " << decoded.decodedText << std::endl;
    return 0;
}

XORResult iterateXORKeys(std::string inputStr) {
    std::string finalStr;
    double lowestFitQuotResult = 100000;
    int bestKey = -1;
    for (int i = 0; i < 256; i++) {     // i == key
        std::string tempStr;
        for (int j = 0; j < inputStr.length(); j++) {
            char c = inputStr[j] ^ i;
            if (std::isalpha(static_cast<unsigned char>(c))) {
                c = std::toupper(c);
            }
            tempStr += c;
        }
        double fitQuotResult = singleStepAnalisys(tempStr);
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

double singleLetterFittingQuotient(int inputStrLetterOccurance, int strLen, char letter) {
    double expected = letterFreqTable(letter) * strLen / 100.0;
    double occured = static_cast<double>(inputStrLetterOccurance);
    if (expected == 0) return 0;
    double fitQuot = std::pow((occured - expected), 2) / expected;
    return abs(fitQuot);
}

double singleStepAnalisys(std::string inputStr) {
    const std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int strLen = inputStr.length();
    double singleStepFittingQuotient = 0;
    for (int i = 0; i < letters.length(); i++) {
        int realOcc = calculateCharOccurance(inputStr, letters[i]);
        singleStepFittingQuotient += singleLetterFittingQuotient(realOcc, strLen, letters[i]);
    }
    return singleStepFittingQuotient;
}

int calculateCharOccurance(std::string inputStr, char c) {
    int counter = 0;
    for (int i = 0; i < inputStr.length(); i++)
        if (inputStr[i] == c)
            counter++;
    //double letterOcc = static_cast<double>(counter) / static_cast<double>(inputStr.length());
    return counter;
}

double letterFreqTable(char c) {
    // Letter frequencies based on this: https://pi.math.cornell.edu/~mec/2003-2004/cryptography/subs/frequencies.html
    std::map<char, float> letterFreq = {
        {'A', 8.12}, {'B', 1.49}, {'C', 2.71}, {'D', 4.32}, {'E', 12.02}, {'F', 2.30}, {'G', 2.03}, {'H', 5.92}, {'I', 7.31},
        {'J', 0.10}, {'K', 0.69}, {'L', 3.98}, {'M', 2.61}, {'N', 6.95}, {'O', 7.68}, {'P', 1.82}, {'Q', 0.11}, {'R', 6.02},
        {'S', 6.28}, {'T', 9.10}, {'U', 2.88}, {'V', 1.11}, {'W', 2.09}, {'X', 0.17}, {'Y', 2.11}, {'Z', 0.07}
    };
    return letterFreq.at(c);
}
