#include <iostream>
#include <fstream> 
#include "xor_utils.h"

int main()
{
    std::string filename = "4.txt";
    std::vector<std::string> inputStrings;
    std::string line;

    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    while (getline(inputFile, line)) {
        inputStrings.push_back(line);
    }
    inputFile.close();

    std::vector<std::string> final;

    int chi2threshold = 60;
    double printableCharTreshhold = 0.8;
    bool additionalInfo = true;

    std::cout << "Candidate decoded strings: " << std::endl;

    for (const auto& input : inputStrings) {
        std::vector<std::string> temp = singleByteXORFreqAnalysis(input, chi2threshold, printableCharTreshhold, additionalInfo);
        final.insert(final.end(), temp.begin(), temp.end());
    }

    for (const auto& s : final) {
        std::cout << s << std::endl;
    }

    return 0;
}
