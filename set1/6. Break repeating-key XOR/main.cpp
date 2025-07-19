#include <iostream>
#include <limits>
#include <vector>
#include <fstream> 
#include <bitset>
#include <algorithm>
#include <filesystem>
#include "converters.h"
#include "xor_utils.h"


int getHammingDistance(std::string_view inputStr1, std::string_view inputStr2);
std::vector<int> findLikelyKeysizes(const std::string_view inputStr, int minKeysize, int maxKeysize, int blockPairCount, int noOfKeys);
std::vector<std::string> transposeVector(const std::vector<std::string>& blocks, int keysize);
std::string getBestKey(const std::vector<std::string>& finalKeys);
std::string getKeyForKeysize(int keysize, const std::string& decodedData);
std::string getDataFromFile();
void saveResultToFile(const std::string& decryptedText);
std::vector<int> getCandidateKeysizes(const std::string& asciiData, int noOfKeysizes);
std::string getFullKeyFromGroupedBlocks(int candidateKeysize, const std::string& asciiData);


// For fast and easy modification of parameters
const int chi2threshold = 40;
const double printableCharTreshhold = 0.7;
const int noOfKeysizes = 3;


int main()
{
    std::string base64Data = getDataFromFile();
    std::string asciiData = base64Toascii(base64Data);

    // Get candidate keysizes
    std::vector<int> candidateKeysizes = getCandidateKeysizes(asciiData, noOfKeysizes);
    std::vector<std::string> finalKeys;

    // Transpose the blocks in encoded text and get key for each group of blocks
    // Try for each candidate keysize
    for (int keysize : candidateKeysizes) {
        finalKeys.push_back(getFullKeyFromGroupedBlocks(keysize, asciiData));
    }

    std::string bestKey = getBestKey(finalKeys);
    std::cout << "\nBest key: " << bestKey << "\n";

    // Final decoded text is written to output.txt
    std::string decryptedText = XOR_repeatingKeyEncrypt(asciiData, bestKey);
    saveResultToFile(decryptedText);

    return 0;
}

std::string getDataFromFile() {
    std::string filename = "6.txt";
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filename);
    }

    std::string base64Data, line;
    while (getline(inputFile, line)) {
        base64Data += line;
    }
    inputFile.close();
    return base64Data;
}

void saveResultToFile(const std::string& decryptedText) {
    std::string filename = "output.txt";
    try {
        std::ofstream output(filename, std::ios::out | std::ios::trunc);
        if (!output) {
            throw std::runtime_error("Can't open file for writing: " + filename);
        }
        output << decryptedText;
        if (!output) {
            throw std::runtime_error("Failed to write to file: " + filename);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}


int getHammingDistance(std::string_view inputStr1, std::string_view inputStr2) {
    size_t len = std::min(inputStr1.size(), inputStr2.size());
    int distance = 0;
    for (size_t i = 0; i < len; i++) {
        distance += std::bitset<8>(inputStr1[i] ^ inputStr2[i]).count();
    }
    return distance;
}

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

std::vector<std::string> transposeVector(const std::vector<std::string>& blocks, int keysize) {
    std::vector<std::string> transposed(keysize);
    for (const auto& block : blocks) {
        for (int i = 0; i < keysize && i < block.size(); ++i) {
            transposed[i] += block[i];
        }
    }
    return transposed;
}

// Find key with lowest Chi^2 - most lkely to be similar to English text
std::string getBestKey(const std::vector<std::string>& finalKeys) {
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

std::string getKeyForKeysize(int keysize, const std::string& decodedData) {

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

std::string getFullKeyFromGroupedBlocks(int candidateKeysize, const std::string& asciiData) {
    std::cout << "\nSingle XOR keys for keysize == " << candidateKeysize << ": \n";
    std::string fullKeyStr = getKeyForKeysize(candidateKeysize, asciiData);
    std::cout << "\nKey for keysize = " << std::to_string(candidateKeysize) << ": " << fullKeyStr << "\n";
    return fullKeyStr;
}