#include <iostream>
#include "xor_utils.h"
#include "converters.h"


int main()
{
    std::string inputStr = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
    std::string key = "ICE";
    std::string encryptedStr = repeatingKeyXOREncrypt(inputStr, key);
    encryptedStr = ASCII2hex(encryptedStr);
    std::cout << "Encoded string (HEX): " << encryptedStr << std::endl;
    return 0;
}
