#ifndef XOR_UTILS_H
#define XOR_UTILS_H

#include <string>
#include <vector>

// ============== XOR RELATED FUNCTIONS ==================
std::string XOR_xorEqualHexString(std::string_view hexString1, std::string_view hexString2);       // Fixed XOR - A function that takes two equal-length hex strings and produces their XOR combination
std::string XOR_xorEqualBinString(std::string_view binString1, std::string_view binString2);   // Fixed XOR - A function that takes two equal-length binary strings and produces their XOR combination
std::vector<std::string> XOR_singleByteFreqAnalysis(std::string_view encodedStr, int chi2threshold, double printableCharTreshhold, bool additionalInfo, bool onlyBestFit);   // Performs a letter frequency analysis on ASCII strings encoded with single-byte XOR cipher (can be used for multiple strings)
// 3 fun below perform a letter frequency analysis on ASCII strings encoded with single-byte XOR cipher (for single string only), they return decoded string(s), key(s) or chi^2 metric
std::vector<std::string> XOR_iterateKeys_str(std::string_view inputStr, int chi2threshold, double printableCharTreshhold, bool additionalInfo, bool onlyBestFit); 
std::vector<int> XOR_iterateKeys_keys(std::string_view inputStr, int chi2threshold, double printableCharTreshhold, bool onlyBestFit);
std::vector<double> XOR_iterateKeys_chi2(std::string_view inputStr, int chi2threshold, double printableCharTreshhold, bool onlyBestFit);
double singleKeyFittingQuotient(std::string_view inputStr);
double singleCharFittingQuotient(int inputStrLetterOccurance, int strLen, char letter);
int countCharOccurance(std::string_view inputStr, char c);
double charFreqTable(char c);
std::string XOR_repeatingKeyEncrypt(std::string_view inputStr, std::string_view key); // XOR encryption with repeating key

#endif // XOR_UTILS_H