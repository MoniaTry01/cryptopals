#ifndef XOR_UTILS_H
#define XOR_UTILS_H

#include <string>
#include <vector>

// ============== XOR RELATED FUNCTIONS ==================
std::string fixedXOR(const std::string& hexString1, const std::string& hexString2);       // Fixed XOR - A function that takes two equal-length hex strings and produces their XOR combination
std::string binStringXOR(const std::string& binString1, const std::string& binString2);   // Fixed XOR - A function that takes two equal-length binary strings and produces their XOR combination
std::vector<std::string> singleByteXORFreqAnalysis(const std::string& encodedStr, int chi2threshold, double printableCharTreshhold, bool additionalInfo);   // Performs a letter frequency analysis on ASCII strings encoded with single-byte XOR cipher (can be used for multiple strings)
std::vector<std::string> iterateXORKeys(const std::string& inputStr, int chi2threshold, double printableCharTreshhold, bool additionalInfo);  // Performs a letter frequency analysis on ASCII strings encoded with single-byte XOR cipher (for single string only)
double singleKeyFittingQuotient(const std::string& inputStr);
double singleCharFittingQuotient(int inputStrLetterOccurance, int strLen, char letter);
int countCharOccurance(const std::string& inputStr, char c);
double charFreqTable(char c);
std::string repeatingKeyXOREncrypt(const std::string& inputStr, const std::string& key); // XOR encryption with repeating key

#endif // XOR_UTILS_H