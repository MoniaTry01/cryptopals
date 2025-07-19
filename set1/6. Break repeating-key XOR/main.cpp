#include <iostream>
#include <limits>
#include <vector>
#include <fstream> 
#include <bitset>
#include <algorithm>
#include <filesystem>
#include "converters.h"
#include "xor_utils.h"


int getHammingDistance(const std::string_view inputStr1, const std::string_view inputStr2);
std::vector<int> bruteForceKeysize(const std::string_view inputStr, int minKeysize, int maxKeysize, int noOfBlocks, int noOfKeys);
std::vector<std::string> transposeVector(const std::vector<std::string>& blocks, int keysize);

int main()
{
    // Get input data from file
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

    // Get potential keysizes
    // The number of candidate keysizes is defined in noOfKeysizes
    std::string decodedData = base64Toascii(base64Data);
    const int minKeysize = 2;
    const int maxKeysize = 40;
    const int noOfBlocks = 8;
    const int noOfKeysizes = 3;
    std::vector<int> candidateKeysizes = bruteForceKeysize(decodedData, minKeysize, maxKeysize, noOfBlocks, noOfKeysizes);

    std::cout << "Candidate keysizes: ";
    for (int keysize : candidateKeysizes) {
        std::cout << keysize << " ";
    }
    std::cout << std::endl;

    // Transpose data vector and get keys for each block
    // Try for each candidate keysize

    std::vector<std::string> finalKeys;
    std::vector<std::string> blocks;

    for (int i = 0; i < noOfKeysizes; ++i) {
        std::cout << std::endl << "Single XOR keys for keysize == " << candidateKeysizes[i] << ": " << std::endl;
        int keysize = candidateKeysizes[i];
        blocks.clear();
        for (size_t j = 0; j + keysize <= decodedData.size(); j += keysize) {
            blocks.push_back(decodedData.substr(j, keysize));
        }

        auto transposedVector = transposeVector(blocks, keysize);

        int chi2threshold = 40;
        double printableCharTreshhold = 0.7;
        bool onlyBestFit = true;
        std::vector<int> singleXORKeys; // keys for each block

        for (const auto& block : transposedVector) {
            std::string_view blockView(block);
            auto key = XOR_iterateKeys_keys(blockView, chi2threshold, printableCharTreshhold, onlyBestFit);
            if (!key.empty()) {
                singleXORKeys.push_back(key[0]);
                std::cout << key[0] << " ";
            }
            else {
                singleXORKeys.clear();
                continue;
            }
        }

        if (singleXORKeys.empty()) {
            std::cout << "Could not find key" << std::endl;
            continue;
        }
        else {
            std::string fullKeyStr; // store full key for current keysize
            fullKeyStr.reserve(singleXORKeys.size());

            for (int k : singleXORKeys) {
                fullKeyStr += static_cast<char>(k);
            }

            std::cout << std::endl << "Key for keysize = " << std::to_string(candidateKeysizes[i]) << ": " << fullKeyStr << std::endl;
            finalKeys.push_back(fullKeyStr);
        }
    }

    double bestKeyChi2 = std::numeric_limits<double>::max();
    std::string bestKey;
    for (auto s : finalKeys) {
        int chi2threshold = 40;
        double printableCharTreshhold = 0.7;
        bool onlyBestFit = true;
        std::vector<double> chi2 = XOR_iterateKeys_chi2(s, chi2threshold, printableCharTreshhold, onlyBestFit);
        if (chi2[0] < bestKeyChi2) {
            bestKeyChi2 = chi2[0];
            bestKey = s;
        }
    }

    std::cout << std::endl << "Best key: " << bestKey << std::endl;

    std::string decryptedText = XOR_repeatingKeyEncrypt(decodedData, bestKey);
    filename = "output.txt";

    try {
        if (std::filesystem::exists(filename)) {
            std::ofstream ofs(filename, std::ios::trunc);
            if (!ofs) {
                throw std::runtime_error("Can't clean the file: " + filename);
            }
        }
        std::ofstream output(filename, std::ios::out);
        if (!output) {
            throw std::runtime_error("Can't open file for writing: " + filename);
        }
        output << decryptedText;
        if (!output) {
            throw std::runtime_error("Failed to write to file: " + filename);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

int getHammingDistance(std::string_view inputStr1, std::string_view inputStr2) {
    size_t len = std::min(inputStr1.size(), inputStr2.size());
    int distance = 0;
    for (size_t i = 0; i < len; i++) {
        distance += std::bitset<8>(inputStr1[i] ^ inputStr2[i]).count();
    }
    return distance;
}

std::vector<int> bruteForceKeysize(const std::string_view inputStr, int minKeysize, int maxKeysize, int noOfBlocks, int noOfKeys) {

    struct result {
        double normDistance;
        int keyLen;
    };

    std::vector<result> results;

    if (inputStr.length() < 2 * maxKeysize)
        maxKeysize = inputStr.length() / 2;

    for (int keysize = minKeysize; keysize <= maxKeysize; keysize++) {
        int blockHamDist = 0;
        for (int i = 0; i < noOfBlocks; i++) {
            if ((2 * keysize * i) + (2 * keysize) > inputStr.length()) break;
            blockHamDist += getHammingDistance(inputStr.substr(2 * keysize * i, keysize), inputStr.substr(2 * keysize * i + keysize, keysize));
        }
        results.push_back({ (double)blockHamDist / (keysize * noOfBlocks), keysize });
    }

    std::sort(results.begin(), results.end(), [](const result& a, const result& b) {
        return a.normDistance < b.normDistance;
    });

    std::vector<int> finalKeysizes;
    for (size_t i = 0; i < noOfKeys; ++i) {
        finalKeysizes.push_back(results[noOfKeys - (i + 1)].keyLen);
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