#ifndef XOR_UTILS_H
#define XOR_UTILS_H

#include <string>
#include <vector>

// =======================
// XOR UTILS HEADER
// Contains functions for performing XOR operations, 
// frequency analysis, and breaking repeating-key XOR encryption.
//
// Functions include:
//  - Fixed XOR on equal-length hex or binary strings
//  - Single-byte XOR cipher frequency analysis and key recovery
//  - Repeating-key XOR encryption and decryption
//  - Breaking repeating-key XOR using Hamming distance and Chi^2 statistics
//=======================

// ============== XOR OPERATIONS ==================

// Performs XOR on two equal-length hex strings
// Input: Two hex strings of the same length
// Output: XOR combination as hex string
std::string XOR_xorEqualHexString(std::string_view hexString1, std::string_view hexString2);

// Performs XOR on two equal-length binary strings
// Input: Two binary strings of the same length
// Output: XOR combination as binary string
std::string XOR_xorEqualBinString(std::string_view binString1, std::string_view binString2);

// ============== SINGLE-BYTE XOR FREQUENCY ANALYSIS ==================

// Performs letter frequency analysis to detect and break single-byte XOR ciphers
// Input: Encoded ASCII string, thresholds, options for filtering results
// Output: Candidate decoded strings matching English frequency statistics
std::vector<std::string> XOR_singleByteFreqAnalysis(std::string_view encodedStr, int chi2threshold, double printableCharTreshhold, bool additionalInfo, bool onlyBestFit);

// Helper functions to iterate over possible single-byte keys and return:
//  - Decoded strings
//  - Keys (as int values)
//  - Chi^2 scores for fit to English text frequencies
std::vector<std::string> XOR_iterateKeys_str(std::string_view inputStr, int chi2threshold, double printableCharTreshhold, bool additionalInfo, bool onlyBestFit);
std::vector<int> XOR_iterateKeys_keys(std::string_view inputStr, int chi2threshold, double printableCharTreshhold, bool onlyBestFit);
std::vector<double> XOR_iterateKeys_chi2(std::string_view inputStr, int chi2threshold, double printableCharTreshhold, bool onlyBestFit);

// Calculates a fitting quotient measuring how well input matches expected English letter frequencies
double singleKeyFittingQuotient(std::string_view inputStr);
double singleCharFittingQuotient(int inputStrLetterOccurance, int strLen, char letter);

// Counts how many times a given character appears in the input string
int countCharOccurance(std::string_view inputStr, char c);

// Returns the expected frequency of a given character in English text
double charFreqTable(char c);

// Performs XOR encryption/decryption with a repeating key
// Input: plaintext/ciphertext string and key string
// Output: XOR-encrypted/decrypted string
std::string XOR_repeatingKeyEncrypt(std::string_view inputStr, std::string_view key);

// ============ FUNCTIONS FOR BREAKING REPEATING KEY XOR ==============

// Breaks repeating-key XOR encryption by:
//  - Finding candidate keysizes via normalized Hamming distance
//  - Extracting candidate keys for each keysizes
//  - Selecting the best key via Chi^2 statistics
//  - Returning decrypted plaintext string
std::string XOR_breakRepeatingKey(const std::string& asciiData, int chi2threshold, int noOfKeysizes, double printableCharTreshhold);

// Computes Hamming distance (bit difference) between two strings
int getHammingDistance(std::string_view inputStr1, std::string_view inputStr2);

// Finds likely keysizes by normalized Hamming distance over multiple blocks
std::vector<int> findLikelyKeysizes(const std::string_view inputStr, int minKeysize, int maxKeysize, int blockPairCount, int noOfKeys);

// Transposes blocks of ciphertext to group bytes encrypted with the same key byte
std::vector<std::string> transposeVector(const std::vector<std::string>& blocks, int keysize);

// Picks the best candidate key from a set based on Chi^2 fit to English letter frequencies
std::string getBestKey(const std::vector<std::string>& finalKeys, int chi2threshold, double printableCharTreshhold);

// Extracts the repeating key for a given keysize by single-byte XOR analysis of transposed blocks
std::string getKeyForKeysize(const std::string& decodedData, int keysize, int chi2threshold, double printableCharTreshhold);

// Returns candidate keysizes based on normalized Hamming distance ranking
std::vector<int> getCandidateKeysizes(const std::string& asciiData, int noOfKeysizes);

// Extracts full repeating key from grouped blocks for a given candidate keysize
std::string getFullKeyFromGroupedBlocks(const std::string& asciiData, int candidateKeysize, int chi2threshold, int noOfKeysizes, double printableCharTreshhold);

#endif // XOR_UTILS_H