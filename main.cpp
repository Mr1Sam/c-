#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

// Structure for BMP file header
#pragma pack(push, 1)
struct BMPHeader {
    char signature[2];
    std::uint32_t fileSize;
    std::uint16_t reserved1;
    std::uint16_t reserved2;
    std::uint32_t dataOffset;
    std::uint32_t headerSize;
    std::int32_t width;
    std::int32_t height;
    std::uint16_t planes;
    std::uint16_t bitsPerPixel;
    std::uint32_t compression;
    std::uint32_t imageSize;
    std::int32_t xPixelsPerM;
    std::int32_t yPixelsPerM;
    std::uint32_t colorsUsed;
    std::uint32_t colorsImportant;
};
#pragma pack(pop)

// Function to rotate image by 90 degrees
void rotateImage(const std::vector<char>& pixelData, std::vector<char>& rotatedPixelData, const BMPHeader& header) {
    for (int y = 0; y < header.height; ++y) {
        for (int x = 0; x < header.width; ++x) {
            int rotatedX = header.height - 1 - y;
            int rotatedY = x;

            // Copy pixel from original data to rotated data
            for (int channel = 0; channel < 3; ++channel) {
                rotatedPixelData[(rotatedY * header.height + rotatedX) * 3 + channel] =
                    pixelData[(y * header.width + x) * 3 + channel];
            }
        }
    }
}

// Function to apply a blur effect to the image
void applyBlur(std::vector<char>& pixelData, const BMPHeader& header) {
    std::vector<char> blurredPixelData(pixelData.size());

    for (int y = 0; y < header.height; ++y) {
        for (int x = 0; x < header.width; ++x) {
            int sumR = 0;
            int sumG = 0;
            int sumB = 0;
            int count = 0;

            // Calculate the average color values of neighboring pixels
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && nx < header.width && ny >= 0 && ny < header.height) {
                        sumR += pixelData[(ny * header.width + nx) * 3];
                        sumG += pixelData[(ny * header.width + nx) * 3 + 1];
                        sumB += pixelData[(ny * header.width + nx) * 3 + 2];
                        count++;
                    }
                }
            }

            // Set the blurred color values to the current pixel
            blurredPixelData[(y * header.width + x) * 3] = static_cast<char>(sumR / count);
            blurredPixelData[(y * header.width + x) * 3 + 1] = static_cast<char>(sumG / count);
            blurredPixelData[(y * header.width + x) * 3 + 2] = static_cast<char>(sumB / count);
        }
    }

    // Copy the blurred pixel data back to the original pixel data
    pixelData = blurredPixelData;
}

int main() {
    const char* inputFileName = "original.bmp";
    const char* outputFileNameRight = "rotated_right.bmp";
    const char* outputFileNameLeft = "rotated_left.bmp";

    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Failed to open the input file." << std::endl;
        return 1;
    }

    BMPHeader header;
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    // Check if the input file is a valid BMP file
    if (header.signature[0] != 'B' || header.signature[1] != 'M') {
        std::cerr << "The input file is not a BMP." << std::endl;
        return 1;
    }

    if (header.bitsPerPixel != 24) {
        std::cerr << "Only images with a color depth of 24 bits are supported." << std::endl;
        return 1;
    }

    // Calculate new dimensions after rotating 90 degrees
    const int newWidth = header.height;
    const int newHeight = header.width;

    // Calculate the size of the new image data
    const std::uint32_t newDataSize = newWidth * newHeight * 3;

    // Update the header with new dimensions
    header.width = newWidth;
    header.height = newHeight;
    header.fileSize = header.dataOffset + newDataSize;

    // Create output files for right and left rotations
    std::ofstream outputFileRight(outputFileNameRight, std::ios::binary);
    std::ofstream outputFileLeft(outputFileNameLeft, std::ios::binary);

     if (!outputFileRight || !outputFileLeft) {
         std::cerr << "Failed to create output files." << std::endl;
         return 1;
     }

     // Write the updated header to the output files
     outputFileRight.write(reinterpret_cast<char*>(&header), sizeof(BMPHeader));
     outputFileLeft.write(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

     // Allocate buffers for pixel data
     std::vector<char> pixelData(newDataSize);
     std::vector<char> rotatedPixelData(newDataSize);

     // Read pixel data from the input file
     inputFile.read(pixelData.data(), pixelData.size());

     // Apply blur effect to the image
     applyBlur(pixelData, header);

     // Rotate the image data and write to output files
     rotateImage(pixelData, rotatedPixelData, header);
     outputFileRight.write(rotatedPixelData.data(), rotatedPixelData.size());

     // Rotate the image data to the left and write to the output file
     rotateImage(rotatedPixelData, pixelData, header);
     outputFileLeft.write(pixelData.data(), pixelData.size());

    return 0;
}