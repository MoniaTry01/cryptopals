#include "xor_utils.h"
#include "converters.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <bitset>

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
        if (fitQuotResult == 0) { continue; };

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

//======== FUNCTIONS FOR BREAKIG REPEATING KEY ENCRYPTION ============

//=============================================
// Break repeating-key XOR encryption
// Takes:
//      asciiData             - encrypted ASCII data
//      chi2threshold         - threshold for Chi^2 filter on key candidates
//      noOfKeysizes          - number of candidate keysizes to try
//      printableCharTreshhold - minimum fraction of printable characters required
// Returns:
//      Decrypted text string
// Note:
//      Detects likely keysizes, extracts possible keys for them,
//      picks the best key based on Chi^2 score and decrypts the input.
//=============================================

std::string XOR_breakRepeatingKey(const std::string& asciiData, int chi2threshold, int noOfKeysizes, double printableCharTreshhold)
{
    // Get candidate keysizes
    std::vector<int> candidateKeysizes = getCandidateKeysizes(asciiData, noOfKeysizes);
    std::vector<std::string> finalKeys;

    // Transpose the blocks in encoded text and get key for each group of blocks
    // Try for each candidate keysize
    for (int keysize : candidateKeysizes) {
        finalKeys.push_back(getFullKeyFromGroupedBlocks(asciiData, keysize, chi2threshold, noOfKeysizes, printableCharTreshhold));
    }

    std::string bestKey = getBestKey(finalKeys, chi2threshold, printableCharTreshhold);
    std::cout << "\nBest key: " << bestKey << "\n";

    // Final decoded text is written to output.txt
    std::string decryptedText = XOR_repeatingKeyEncrypt(asciiData, bestKey);
    return decryptedText;
}


//=============================================
// Compute Hamming distance (bit difference) between two strings
// Takes:
//      inputStr1 - first input string
//      inputStr2 - second input string
// Returns:
//      Hamming distance (number of differing bits)
// Note:
//      Used for guessing keysize by comparing blocks
//=============================================

int getHammingDistance(std::string_view inputStr1, std::string_view inputStr2) {
    size_t len = std::min(inputStr1.size(), inputStr2.size());
    int distance = 0;
    for (size_t i = 0; i < len; i++) {
        distance += std::bitset<8>(inputStr1[i] ^ inputStr2[i]).count();
    }
    return distance;
}


//=============================================
// Find likely keysizes based on normalized Hamming distance
// Takes:
//      inputStr      - ASCII input string
//      minKeysize    - minimal keysize to try
//      maxKeysize    - maximal keysize to try
//      blockPairCount - number of block pairs to compare per keysize
//      noOfKeys      - number of top keysizes to return
// Returns:
//      Vector of likely keysizes (sorted by normalized distance)
//=============================================

std::vector<int> findLikelyKeysizes(const std::string_view inputStr, int minKeysize, int maxKeysize, int blockPairCount, int noOfKeys) {

    struct result {
        double normDistance;
        int keyLen;
    };

    std::vector<result> results;

    if (inputStr.length() < 2 * maxKeysize)
        maxKeysize = inputStr.length() / 2;

    for (int keysize = minKeysize; keysize <= maxKeysize; keysize++) {
        int blockHamDist = 0;
        for (int i = 0; i < blockPairCount; i++) {
            if ((2 * keysize * i) + (2 * keysize) > inputStr.length()) break;
            blockHamDist += getHammingDistance(inputStr.substr(2 * keysize * i, keysize), inputStr.substr(2 * keysize * i + keysize, keysize));
        }
        results.push_back({ static_cast<double>(blockHamDist) / (keysize * blockPairCount), keysize });
    }

    std::sort(results.begin(), results.end(), [](const result& a, const result& b) {
        return a.normDistance < b.normDistance;
        });

    // Get [noOfKeys] keysizes with lowest normalized Hamming distance
    std::vector<int> finalKeysizes;
    for (size_t i = 0; i < std::min(results.size(), static_cast<size_t>(noOfKeys)); ++i) {
        finalKeysizes.push_back(results[i].keyLen);
    }

    return finalKeysizes;
}


//=============================================
// Transpose blocks of text for repeating-key XOR analysis
// Takes:
//      blocks  - vector of blocks of size == keysize
//      keysize - size of each block
// Returns:
//      Vector of transposed strings (each contains bytes XOR-ed with the same key byte)
//=============================================

std::vector<std::string> transposeVector(const std::vector<std::string>& blocks, int keysize) {
    std::vector<std::string> transposed(keysize);
    for (const auto& block : blocks) {
        for (int i = 0; i < keysize && i < block.size(); ++i) {
            transposed[i] += block[i];
        }
    }
    return transposed;
}


//=============================================
// Pick the best key from candidates based on Chi^2 score
// Takes:
//      finalKeys             - vector of extracted keys
//      chi2threshold         - Chi^2 threshold for key scoring
//      printableCharTreshhold - printable characters threshold
// Returns:
//      Best fitting key as a string
//=============================================

std::string getBestKey(const std::vector<std::string>& finalKeys, int chi2threshold, double printableCharTreshhold) {
    double bestKeyChi2 = std::numeric_limits<double>::max();
    std::string bestKey;
    const bool onlyBestFit = true;

    for (const auto& s : finalKeys) {
        auto chi2 = XOR_iterateKeys_chi2(s, chi2threshold, printableCharTreshhold, onlyBestFit);
        if (!chi2.empty() && chi2[0] < bestKeyChi2) {
            bestKeyChi2 = chi2[0];
            bestKey = s;
        }
    }
    return bestKey;
}


//=============================================
// Get key for given keysize by single-byte XOR analysis on transposed blocks
// Takes:
//      decodedData           - ASCII input data
//      keysize               - keysize to test
//      chi2threshold         - Chi^2 threshold for key candidates
//      printableCharTreshhold - printable characters threshold
// Returns:
//      Key string for given keysize (or empty if failed)
//=============================================

std::string getKeyForKeysize(const std::string& decodedData, int keysize, int chi2threshold, double printableCharTreshhold) {

    std::string fullKeyStr; // store full key for current keysize
    std::vector<std::string> blocks;

    for (size_t j = 0; j + keysize <= decodedData.size(); j += keysize) {   // separate the text in blocks, block length == keysize
        blocks.push_back(decodedData.substr(j, keysize));
    }

    auto transposedVector = transposeVector(blocks, keysize);   // realign blocks, so the blocks that can be deciphered by the same single-byte key are grouped
    const bool onlyBestFit = true;
    std::vector<int> singleXORKeys; // keys for each group of blocks

    for (const auto& block : transposedVector) {
        std::string_view blockView(block);
        auto key = XOR_iterateKeys_keys(blockView, chi2threshold, printableCharTreshhold, onlyBestFit); // find key for a group of blocks
        if (!key.empty()) {                         // if at some point key is returned empty, that means that XOR_iterateKeys_keys couldn't find the key
            singleXORKeys.push_back(key[0]);        // with sufficiently low chi^2 metric, which means finding key is impossible for given thresholds
            std::cout << key[0] << " ";             // Break the loop, to avoid returning incomplete key
        }
        else {
            std::cout << "\nWarning: Key extraction failed for block.\n";
            singleXORKeys.clear();
            break;
        }
    }

    if (singleXORKeys.empty()) {
        fullKeyStr.clear();
    }
    else {
        fullKeyStr.reserve(singleXORKeys.size());

        for (int k : singleXORKeys) {
            fullKeyStr += static_cast<char>(k);
        }
    }

    return fullKeyStr;
}


//=============================================
// Get candidate keysizes based on normalized Hamming distance analysis
// Takes:
//      asciiData   - ASCII input string
//      noOfKeysizes - number of candidate keysizes to return
// Returns:
//      Vector of candidate keysizes
//============================================

std::vector<int> getCandidateKeysizes(const std::string& asciiData, int noOfKeysizes) {
    // The number of candidate keysizes is defined by noOfKeysizes
    const int minKeysize = 2;
    const int maxKeysize = 40;
    const int noOfBlockPairs = 4;
    std::vector<int> candidateKeysizes = findLikelyKeysizes(asciiData, minKeysize, maxKeysize, noOfBlockPairs, noOfKeysizes);

    std::cout << "Candidate keysizes: ";
    for (int keysize : candidateKeysizes) {
        std::cout << keysize << " ";
    }
    std::cout << "\n";
    return candidateKeysizes;
}


//=============================================
// Extract full key for given keysize by analyzing grouped blocks
// Takes:
//      asciiData             - ASCII input data
//      candidateKeysize      - tested keysize
//      chi2threshold         - Chi^2 threshold for single-byte XOR analysis
//      noOfKeysizes          - unused (kept for compatibility)
//      printableCharTreshhold - printable characters threshold
// Returns:
//      Extracted key string for given keysize
//=============================================

std::string getFullKeyFromGroupedBlocks(const std::string& asciiData, int candidateKeysize, int chi2threshold, int noOfKeysizes, double printableCharTreshhold) {
    std::cout << "\nSingle XOR keys for keysize == " << candidateKeysize << ": \n";
    std::string fullKeyStr = getKeyForKeysize(asciiData, candidateKeysize, chi2threshold, printableCharTreshhold);
    std::cout << "\nKey for keysize = " << std::to_string(candidateKeysize) << ": " << fullKeyStr << "\n";
    return fullKeyStr;
}