#include <iostream>
#include <fstream>
#include <iomanip>

void binaryToHex(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream binFile(inputFile, std::ios::binary);
    if (!binFile) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        return;
    }

    std::ofstream hexFile(outputFile);
    if (!hexFile) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        return;
    }

    char byte;
    while (binFile.get(byte)) {
        hexFile << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(byte) & 0xFF);
    }

    std::cout << "Hex data successfully written to " << outputFile << std::endl;
}

int main() {
    std::string inputFile = "test.bin";
    std::string outputFile = "outhex.txt";
    binaryToHex(inputFile, outputFile);
    return 0;
}