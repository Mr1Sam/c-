#include <iostream>
#include <fstream>
#include <vector>

void rotateImageCCW(std::vector<unsigned char>& input, std::vector<unsigned char>& output, int width, int height) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            output[i + j * height] = input[j + (height - i - 1) * width];
        }
    }
}

int main() {
    // Load BMP image
    std::ifstream inputFile("bmp", std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening input BMP file!" << std::endl;
        return 1;
    }

    // Read BMP header (skip)
    inputFile.seekg(18, std::ios::beg);

    int width, height;
    inputFile.read(reinterpret_cast<char*>(&width), 4);
    inputFile.read(reinterpret_cast<char*>(&height), 4);

    std::vector<unsigned char> inputImage(width * height);
    inputFile.read(reinterpret_cast<char*>(inputImage.data()), width * height);
    inputFile.close();

    std::vector<unsigned char> rotatedImage(width * height);


    rotateImageCCW(inputImage, rotatedImage, width, height);

    std::ofstream outputFile("rotated.bmp", std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening output BMP file!" << std::endl;
        return 1;
    }

    // BMP header
    outputFile.write("BM", 2);
    int fileSize = 54 + width * height;
    outputFile.write(reinterpret_cast<const char*>(&fileSize), 4);
    int reserved = 0;
    outputFile.write(reinterpret_cast<const char*>(&reserved), 4);
    int dataOffset = 54;
    outputFile.write(reinterpret_cast<const char*>(&dataOffset), 4);

    // width and height
    outputFile.write(reinterpret_cast<const char*>(&width), 4);
    outputFile.write(reinterpret_cast<const char*>(&height), 4);

    // remaining BMP header
    outputFile.write("\x01\x00\x08\x00", 4); // 1 plane, 8 bits per pixel
    int compression = 0;
    outputFile.write(reinterpret_cast<const char*>(&compression), 4); // no compression
    int dataSize = width * height;
    outputFile.write(reinterpret_cast<const char*>(&dataSize), 4);
    outputFile.write("\x13\x0B\x00\x00\x13\x0B\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16); // Default values for BMP
    outputFile.write(reinterpret_cast<const char*>(rotatedImage.data()), width * height);

    outputFile.close();

    return 0;
}
